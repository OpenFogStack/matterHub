#include "MQTTCommands.h"
#include "MQTTManager.h"
#include "esp_log.h"
namespace matterHub {
class CloudConnector
{
public:
    CloudConnector(){};

    static void DCMD_callback() { ESP_LOGI("Cloud Connector", "TEST"); }
    static void InitDCMD()
    {
        shell::MQTTCommandData * data = chip::Platform::New<shell::MQTTCommandData>();
        data->topic                   = "/test";
        data->task                    = shell::MQTTCommandTask::subscribe;
        chip::MQTTManager::GetInstance().Subscribe(data, DCMD_callback);
    }

private:
};
} // namespace matterHub