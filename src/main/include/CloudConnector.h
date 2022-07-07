#include "InteractionModelHelper.h"
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
    static void onAttributeReadCallback(const chip::app::ConcreteDataAttributePath & path, chip::TLV::TLVReader * data,
                                        void * context)
    {
        ESP_LOGI(TAG, "Context: %p", context);
        Subscription * subscription = reinterpret_cast<Subscription *>(context);
        ESP_LOGI(TAG, "This: %p", subscription);
        ESP_LOGI(TAG, "mdevice: %p", subscription->mDevice);
        ESP_LOGI(TAG, "Nodeid: %llu", subscription->mDevice->GetDeviceId());

        data->GetContainerType();
        data->GetType();
        data->GetLengthRead();
        uint16_t controlByte = data->GetControlByte();
        /* May removed in the comming releases of ConnectedHomeIP */
        chip::TLV::TLVElementType elemType;
        /* May switch to GetType()? */
        if (controlByte == static_cast<uint16_t>(chip::TLV::kTLVControlByte_NotSpecified))
        {
            ESP_LOGE(TAG, "control Byte not specified");
            return;
        }

        char * topic = (char *) chip::Platform::MemoryAlloc(sizeof(char) * 256);
        snprintf(topic, 256, "spBv1.0/matterhub/DDATA/0/%llu", subscription->mDevice->GetDeviceId());
        char name[256] = { 0 };
        snprintf(name, sizeof(name), "%u/%u/%u", path.mEndpointId, path.mClusterId, path.mAttributeId);
        cJSON * root;
        root = cJSON_CreateObject();
        cJSON_AddNumberToObject(root, "timestamp", esp_log_timestamp());
        cJSON * metrics;
        metrics = cJSON_AddArrayToObject(root, "metrics");
        cJSON * element;
        element = cJSON_CreateObject();
        cJSON_AddStringToObject(element, "name", name);
        cJSON_AddNumberToObject(element, "timestamp", esp_log_timestamp());
        elemType = static_cast<chip::TLV::TLVElementType>(controlByte & chip::TLV::kTLVTypeMask);

        switch (elemType)
        {
        case chip::TLV::TLVElementType::NotSpecified: {
            break;
        }
        case chip::TLV::TLVElementType::Int8: {
            int8_t value;
            data->Get(value);
            cJSON_AddStringToObject(element, "dataType", "Int8");
            cJSON_AddNumberToObject(element, "value", value);
            break;
        }
        case chip::TLV::TLVElementType::Int16: {
            int16_t value;
            data->Get(value);
            cJSON_AddStringToObject(element, "dataType", "Int16");
            cJSON_AddNumberToObject(element, "value", value);
            break;
        }
        case chip::TLV::TLVElementType::Int32: {
            int32_t value;
            data->Get(value);
            cJSON_AddStringToObject(element, "dataType", "Int32");
            cJSON_AddNumberToObject(element, "value", value);
            break;
        }
        case chip::TLV::TLVElementType::Int64: {
            int64_t value;
            data->Get(value);
            cJSON_AddStringToObject(element, "dataType", "Int64");
            cJSON_AddNumberToObject(element, "value", value);
            break;
        }
        case chip::TLV::TLVElementType::UInt8: {
            uint8_t value;
            data->Get(value);
            cJSON_AddStringToObject(element, "dataType", "uInt8");
            cJSON_AddNumberToObject(element, "value", value);
            break;
        }
        case chip::TLV::TLVElementType::UInt16: {
            uint16_t value;
            data->Get(value);
            cJSON_AddStringToObject(element, "dataType", "uInt16");
            cJSON_AddNumberToObject(element, "value", value);
            break;
        }
        case chip::TLV::TLVElementType::UInt32: {
            uint32_t value;
            data->Get(value);
            cJSON_AddStringToObject(element, "dataType", "uInt32");
            cJSON_AddNumberToObject(element, "value", value);
            break;
        }
        case chip::TLV::TLVElementType::UInt64: {
            uint64_t value;
            data->Get(value);
            cJSON_AddStringToObject(element, "dataType", "uInt64");
            cJSON_AddNumberToObject(element, "value", value);
            break;
        }
        case chip::TLV::TLVElementType::BooleanFalse: {
            cJSON_AddStringToObject(element, "dataType", "Boolean");
            cJSON_AddBoolToObject(element, "value", false);
            break;
        }

        case chip::TLV::TLVElementType::BooleanTrue: {
            cJSON_AddStringToObject(element, "dataType", "Boolean");
            cJSON_AddBoolToObject(element, "value", true);
            break;
        }
        case chip::TLV::TLVElementType::FloatingPointNumber32: {
            float value;
            data->Get(value);
            cJSON_AddStringToObject(element, "dataType", "float");
            cJSON_AddNumberToObject(element, "value", value);
            break;
        }
        case chip::TLV::TLVElementType::FloatingPointNumber64: {
            double value;
            cJSON_AddStringToObject(element, "dataType", "double");
            cJSON_AddNumberToObject(element, "value", value);
            break;
        }
        case chip::TLV::TLVElementType::UTF8String_1ByteLength:
            break;
        case chip::TLV::TLVElementType::UTF8String_2ByteLength:
            break;
        case chip::TLV::TLVElementType::UTF8String_4ByteLength:
            break;
        case chip::TLV::TLVElementType::UTF8String_8ByteLength:
            break;
        case chip::TLV::TLVElementType::ByteString_1ByteLength:
        case chip::TLV::TLVElementType::ByteString_2ByteLength:
        case chip::TLV::TLVElementType::ByteString_4ByteLength:
        case chip::TLV::TLVElementType::ByteString_8ByteLength: {
            uint16_t length = data->GetLength();
            /* TODO Overflow check? */
            length++;
            char * value = (char *) chip::Platform::MemoryAlloc(sizeof(char) * length);
            data->GetString(value, length);
            cJSON_AddStringToObject(element, "dataType", "String");
            cJSON_AddStringToObject(element, "value", value);
            chip::Platform::Delete(value);
            break;
        }
        // IMPORTANT: Values starting at Null must match the corresponding values of
        // TLVType.
        case chip::TLV::TLVElementType::Null:
            break;
        case chip::TLV::TLVElementType::Structure:
            break;
        case chip::TLV::TLVElementType::Array:
            break;
        case chip::TLV::TLVElementType::List:
            break;
        case chip::TLV::TLVElementType::EndOfContainer:
            break;
        }

        // TODO: What we want to use:
        cJSON_AddItemToArray(metrics, element);
        char * my_json_string = cJSON_Print(root);
        cJSON_Delete(root);

        shell::MQTTCommandData * mqttCommand = chip::Platform::New<shell::MQTTCommandData>();
        mqttCommand->topic                   = topic;
        mqttCommand->data                    = my_json_string;
        mqttCommand->task                    = shell::MQTTCommandTask::publish;
        chip::MQTTManager::GetInstance().ProcessCommand(mqttCommand);
    }
    // TODO handle data_total length...
    static void DCMD_callback(char * topic, int topic_len, char * data, int data_len)
    {
        ESP_LOGI(TAG, "DCMD Callback");
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
            if (!action.compare("cmd"))
            {
                CHIP_ERROR error = chip::InteractionModelHelper::command(nodeId, endpointId, clusterId, action_specific_id);
            }

            if (!action.compare("read"))
            {
                CHIP_ERROR error =
                    chip::InteractionModelHelper::read(nodeId, endpointId, clusterId, action_specific_id, &onAttributeReadCallback);
            }
            if (!action.compare("subscribe"))
            {
                CHIP_ERROR error = chip::InteractionModelHelper::subscribe(nodeId, endpointId, clusterId, action_specific_id, 1, 10,
                                                                           &onAttributeReadCallback);
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
        char * topic                  = (char *) chip::Platform::MemoryAlloc(sizeof(char) * 256);
        snprintf(topic, 256, "spBv1.0/matterhub/DCMD/%d/+", CONFIG_MATTERHUBID);
        data->topic = topic;
        data->task  = shell::MQTTCommandTask::subscribe;
        chip::MQTTManager::GetInstance().Subscribe(data, DCMD_callback);
    }

private:
};
} // namespace matterHub