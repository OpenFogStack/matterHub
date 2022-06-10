#include "MQTTManager.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "nvs_flash.h"
//#include "protocol_examples_common.h"

#include "esp_log.h"
#include "esp_ota_ops.h"
//#include "esp_tls.h"
//#include "mqtt_client.h"
#include <sys/param.h>

namespace chip {
MQTTManager MQTTManager::sMQTTManager;

void MQTTManager::SendPub(shell::MQTTCommandData * data) {}
void initMQTTManager()
{
    /*
    const esp_mqtt_client_config_t mqtt_cfg = {
        .uri      = "***REMOVED***",
        .cert_pem = (const char *) mqtt_eclipseprojects_io_pem_start,
        .username = "Someone",
        .password = "***REMOVED***",
    };

    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
    */
}
} // namespace chip