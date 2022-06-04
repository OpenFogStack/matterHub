#include "platform/CHIPDeviceLayer.h"
#include <MQTTClient.h>

constexpr char * module = "MQTTClient";

MQTTClient::MQTTClient()
{
    CHIP_ERROR err = CHIP_NO_ERROR;
    err            = chip::DeviceLayer::TCPEndPointManager()->NewEndPoint(&mEndpoint);
    VerifyOrExit(err == CHIP_NO_ERROR, ESP_LOGE(module, "Failed to init TCP Endpoint: %s\n", ErrorStr(err)));
exit:
    if ((err != CHIP_NO_ERROR))
    {
        ESP_LOGE(module, "Send failed with error: %s\n", ErrorStr(err));
    }
}