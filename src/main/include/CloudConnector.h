#include "MQTTCommands.h"
#include "MQTTManager.h"
#include "cJSON.h"
#include "esp_log.h"
#include <vector>
namespace matterHub {
static const char * TAG = "Cloud Connector";
class CloudConnector
{

public:
    CloudConnector(){};

    // TODO handle data_total length...
    static void DCMD_callback(char * topic, int topic_len, char * data, int data_len)
    {
        ESP_LOGI(TAG, "DCMD Callback");
        printf("TOPIC=%.*s\r\n", topic_len, topic);
        printf("DATA=%.*s\r\n", data_len, data);
        /* Parse topic */
        std::string s_topic   = std::string(topic, topic_len);
        std::string delimiter = "/";

        size_t pos = 0;
        std::vector<std::string> topic_split;
        std::string token;
        while ((pos = s_topic.find(delimiter)) != std::string::npos)
        {
            token = s_topic.substr(0, pos);
            ESP_LOGI(TAG, "TOKEN: %s", token.c_str());
            s_topic = s_topic.substr(pos + delimiter.length());
            topic_split.push_back(token);
        }
        token = s_topic;
        ESP_LOGI(TAG, "TOKEN: %s", token.c_str());
        topic_split.push_back(token);
        if (topic_split.size() < 5)
        {
            ESP_LOGE(TAG, "Topic %.*s\r\n was malformed, no NodeID", topic_len, topic);
            return;
        }
        chip::NodeId nodeId = std::stoull(topic_split[4]);
        ESP_LOGI("Cloud Connector", "NodeID: %llu", nodeId);

        /* Parse Data */
        cJSON * root2 = cJSON_ParseWithLength(data, data_len);
        if (cJSON_GetObjectItem(root2, "timestamp"))
        {
            unsigned long timestamp = cJSON_GetObjectItem(root2, "timestamp")->valueint;
            ESP_LOGI(TAG, "timestamp=%lu", timestamp);
        }
        else
        {
            ESP_LOGE(TAG, "timestamp is missing!");
        }

        cJSON * metrics  = cJSON_GetObjectItem(root2, "metrics");
        int metrics_size = cJSON_GetArraySize(metrics);
        ESP_LOGI(TAG, "metrics_size=%d", metrics_size);
        for (int i = 0; i < metrics_size; i++)
        {
            cJSON * element2 = cJSON_GetArrayItem(metrics, i);
            // cJSON_Print(element2);
            if (cJSON_GetObjectItem(element2, "timestamp"))
            {
                unsigned long timestamp = cJSON_GetObjectItem(element2, "timestamp")->valueint;
                ESP_LOGI(TAG, "timestamp=%lu", timestamp);
            }
            if (!cJSON_GetObjectItem(element2, "name"))
            {
                ESP_LOGI(TAG, "DCMD did not contain a name");
                continue;
            }
            char * name = cJSON_GetObjectItem(element2, "name")->valuestring;
            ESP_LOGI(TAG, "name=%s", name);

            std::vector<std::string> name_split;
            std::string s_name = name;
            std::string token;
            std::string delimiter = "/";
            while ((pos = s_name.find(delimiter)) != std::string::npos)
            {
                token = s_name.substr(0, pos);
                ESP_LOGI(TAG, "TOKEN: %s", token.c_str());
                s_name = s_name.substr(pos + delimiter.length());
                name_split.push_back(token);
            }
            token = s_name;
            ESP_LOGI(TAG, "TOKEN: %s", token.c_str());
            name_split.push_back(token);
            if (name_split.size() != 4)
            {
                ESP_LOGE(TAG, "name %s\r\n was malformed", name);
                continue;
            }

            /* TODO: handle stuff if someone provides non ints*/
            chip::EndpointId endpointId = std::stoi(name_split[0]);
            chip::ClusterId clusterId   = std::stoi(name_split[1]);
            std::string action          = name_split[2];
            int action_specific_id      = std::stoi(name_split[3]);

            /* OnOff : OnOff*/
            ESP_LOGE(TAG, "Endpoint %d", endpointId);
            ESP_LOGE(TAG, "Cluster %u", clusterId);
            ESP_LOGE(TAG, "action %s", action.c_str());
            ESP_LOGE(TAG, "action_specific_id: %d", action_specific_id);
            if (clusterId == 6)
            {
                ESP_LOGI(TAG, "CLUSTER OnOff");
                if (!action.compare("cmd"))
                {
                    ESP_LOGI(TAG, "Server sent command");
                    switch (action_specific_id)
                    {
                    case 0:
                        ESP_LOGI(TAG, "command: Off");
                        break;

                    case 1:
                        ESP_LOGI(TAG, "command: On");
                        break;
                    case 2:
                        ESP_LOGI(TAG, "command: toggle");
                        break;
                    default:
                        break;
                    }
                }
            }
        }

        /*int cores = cJSON_GetObjectItem(root2, "cores")->valueint;
        ESP_LOGI(TAG, "cores=%d", cores);
        cJSON * array2  = cJSON_GetObjectItem(root2, "array");
        int array2_size = cJSON_GetArraySize(array2);
        ESP_LOGI(TAG, "array2_size=%d", array2_size);
        for (int i = 0; i < array2_size; i++)
        {
            cJSON * element2 = cJSON_GetArrayItem(array2, i);
            JSON_Print(element2);
        }*/
        cJSON_Delete(root2);
    }
    static void InitDCMD()
    {
        shell::MQTTCommandData * data = chip::Platform::New<shell::MQTTCommandData>();
        data->topic                   = "spBv1.0/matterhub/DCMD/0/+";
        data->task                    = shell::MQTTCommandTask::subscribe;
        chip::MQTTManager::GetInstance().Subscribe(data, DCMD_callback);
    }

private:
};
} // namespace matterHub