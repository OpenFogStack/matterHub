/*
 *    Published under MIT License
 *    Copyright (c) 2022 OpenFogStack
 *    Based on: https://github.com/espressif/esp-idf/tree/c2ccc383da/examples/protocols/mqtt/ssl, which is in the Public Domain
 */
#include "MQTTManager.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "nvs_flash.h"

#include "cJSON.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_tls.h"
#include "mqtt_client.h"
#include <map>
#include <sys/param.h>
static const char * TAG = "MQTT";

/* Most of the MQTT code is taken from the ESP-Idf mqtt example:
https://github.com/espressif/esp-idf/tree/01d014c42d/examples/protocols/mqtt/ssl

*/

#if CONFIG_BROKER_CERTIFICATE_OVERRIDDEN == 1
static const uint8_t mqtt_hivemq_pem_start[] =
    "-----BEGIN CERTIFICATE-----\n" CONFIG_BROKER_CERTIFICATE_OVERRIDE "\n-----END CERTIFICATE-----";
#else
extern const uint8_t mqtt_hivemq_pem_start[] asm("_binary_mqtt_hivemq_pem_start");
#endif
extern const uint8_t mqtt_hivemq_pem_end[] asm("_binary_mqtt_hivemq_pem_end");

namespace chip {
MQTTManager MQTTManager::sMQTTManager;
bool MQTTManager::mInit = false;
bool mConnected         = false;
std::map<std::string, std::function<void(char *, int, char *, int)>> topicCallbacks;
std::vector<shell::MQTTCommandData *> mStoredCommands;
esp_mqtt_client_handle_t mClient;

void MQTTManager::Publish(shell::MQTTCommandData * data)
{
    if (!mConnected)
    {
        ESP_LOGI(TAG, "Currently not connected, delay command until connection is established (again)");
        return;
    }
    int msg_id;
    msg_id = esp_mqtt_client_publish(mClient, data->topic, data->data, 0, 0, 0);
    ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
    cJSON_free(data->data);
    Platform::Delete(data->topic);
    Platform::Delete(data);
}
void MQTTManager::Subscribe(shell::MQTTCommandData * data, std::function<void(char *, int, char *, int)> callback)
{
    std::string key(data->topic);
    ESP_LOGI(TAG, "key is: %s", key.c_str());
    topicCallbacks[key] = callback;
    if (!mConnected)
    {
        ESP_LOGI(TAG, "Currently not connected, delay command until connection is established (again)");
        mStoredCommands.push_back(data);
        return;
    }
    int msg_id;
    msg_id = esp_mqtt_client_subscribe(mClient, data->topic, 0);
    ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
    Platform::Delete(data);
}

void MQTTManager::Subscribe(shell::MQTTCommandData * data)
{
    if (!mConnected)
    {
        ESP_LOGI(TAG, "Currently not connected, delay command until connection is established (again)");
        mStoredCommands.push_back(data);
        return;
    }
    int msg_id;
    msg_id = esp_mqtt_client_subscribe(mClient, data->topic, 0);
    ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
    Platform::Delete(data->topic);
    Platform::Delete(data);
}
void MQTTManager::Unsubscribe(shell::MQTTCommandData * data)
{
    if (!mConnected)
    {
        ESP_LOGI(TAG, "Currently not connected, delay command until connection is established (again)");
        mStoredCommands.push_back(data);
        return;
    }
    int msg_id;
    msg_id = esp_mqtt_client_unsubscribe(mClient, data->topic);
    ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
    Platform::Delete(data);
}

void MQTTManager::ProcessCommand(shell::MQTTCommandData * data)
{
    switch (data->task)
    {
    case shell::MQTTCommandTask::subscribe:
        chip::MQTTManager::GetInstance().Subscribe(data);
        ESP_LOGI(TAG, "Subscribe via cmd is no longer supported");
        break;
    case shell::MQTTCommandTask::unsubscribe:
        chip::MQTTManager::GetInstance().Unsubscribe(data);
        break;
    case shell::MQTTCommandTask::publish:
        chip::MQTTManager::GetInstance().Publish(data);
        break;
    default:
        ChipLogError(NotSpecified, "MQTTCommandWorkerFunction - Invalid Task");
        break;
    }
}

//
// Note: this function is for testing purposes only publishing part of the active partition
//       (to be checked against the original binary)
//
static void send_binary(esp_mqtt_client_handle_t client)
{
    const char * test_data = "test";
    int msg_id             = esp_mqtt_client_publish(client, "/topic/binary", (const char *) test_data, strlen(test_data), 0, 0);
    ESP_LOGI(TAG, "binary sent with msg_id=%d", msg_id);
}

/*
 * @brief Event handler registered to receive MQTT events
 *
 *  This function is called by the MQTT client event loop.
 *
 * @param handler_args user data registered to the event.
 * @param base Event base for the handler(always MQTT Base in this example).
 * @param event_id The id for the received event.
 * @param event_data The data for the event, esp_mqtt_event_handle_t.
 */
static void mqtt_event_handler(void * handler_args, esp_event_base_t base, int32_t event_id, void * event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    esp_mqtt_event_handle_t event   = (esp_mqtt_event_handle_t) event_data;
    esp_mqtt_client_handle_t client = event->client;
    switch ((esp_mqtt_event_id_t) event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        mConnected = true;
        for (auto & storedCommand : mStoredCommands)
        {
            // Ugly edge-case, but this could happen I guess
            if (!mConnected)
            {
                break;
            }
            ESP_LOGI(TAG, "Processing delay command:");
            ESP_LOGI(TAG, " - Topic: '%s'", storedCommand->topic);
            if (storedCommand->data)
            {
                ESP_LOGI(TAG, " - Data: '%s'", storedCommand->data);
            }
            MQTTManager::GetInstance().ProcessCommand(storedCommand);
        }
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        mConnected = false;
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA: {
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        if (strncmp(event->data, "send binary please", event->data_len) == 0)
        {
            ESP_LOGI(TAG, "Sending the binary");
            send_binary(client);
        }
        std::string key(event->topic, event->topic_len);
        ESP_LOGI(TAG, "key is: %s", key.c_str());

        for (std::pair<std::string, std::function<void(char *, int, char *, int)>> element : topicCallbacks)
        {
            // TODO match that pattern...
            element.second(event->topic, event->topic_len, event->data, event->data_len);
        }

        break;
    }
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
        {
            ESP_LOGI(TAG, "Last error code reported from esp-tls: 0x%x", event->error_handle->esp_tls_last_esp_err);
            ESP_LOGI(TAG, "Last tls stack error number: 0x%x", event->error_handle->esp_tls_stack_err);
            ESP_LOGI(TAG, "Last captured errno : %d (%s)", event->error_handle->esp_transport_sock_errno,
                     strerror(event->error_handle->esp_transport_sock_errno));
        }
        else if (event->error_handle->error_type == MQTT_ERROR_TYPE_CONNECTION_REFUSED)
        {
            ESP_LOGI(TAG, "Connection refused error: 0x%x", event->error_handle->connect_return_code);
        }
        else
        {
            ESP_LOGW(TAG, "Unknown error type: 0x%x", event->error_handle->error_type);
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

void MQTTManager::initMQTTManager()
{
    const esp_mqtt_client_config_t mqtt_cfg = {
        .uri      = CONFIG_MQTT_CLIENT_URI,
        .username = CONFIG_MQTT_CLIENT_USERNAME,
        .password = CONFIG_MQTT_CLIENT_PASSWORD,
        .cert_pem = (const char *) mqtt_hivemq_pem_start,

    };
    ESP_LOGI(TAG, "This is my name: %s and this is my key %s", CONFIG_MQTT_CLIENT_USERNAME, CONFIG_MQTT_CLIENT_PASSWORD);
    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    mClient = esp_mqtt_client_init(&mqtt_cfg);

    esp_mqtt_client_register_event(mClient, ((esp_mqtt_event_id_t) ESP_EVENT_ANY_ID), mqtt_event_handler, NULL);
    esp_mqtt_client_start(mClient);
}

} // namespace chip
