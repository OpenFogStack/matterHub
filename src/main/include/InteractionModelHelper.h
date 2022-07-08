#pragma once

#include "ConnectionHelper.h"
#include "InteractionModelCommands.h"
#include "Subscription.h"
#include "platform/CHIPDeviceLayer.h"
#include <InteractionModel.h>
#include <app/OperationalDeviceProxy.h>

namespace chip {

class InteractionModelHelper : public InteractionModelReports, public chip::app::ReadClient::Callback
{
public:
    InteractionModelHelper();

    static CHIP_ERROR subscribe(NodeId nodeId, EndpointId endpointId, ClusterId clusterId, AttributeId attributeId,
                                uint16_t minInterval, uint16_t maxInterval, SubscriptionCallback callback);
    static CHIP_ERROR command(NodeId nodeId, EndpointId endpointId, ClusterId clusterId, CommandId commandId);

    static CHIP_ERROR read(NodeId nodeId, EndpointId endpointId, ClusterId clusterId, AttributeId attributeId,
                           SubscriptionCallback callback);

    template <typename T>
    static CHIP_ERROR write(NodeId nodeId, EndpointId endpointId, ClusterId clusterId, AttributeId attributeId, T & value)
    {
        WriteCommandData<T> * data = Platform::New<WriteCommandData<T>>(value);
        data->fabricId             = 1;
        data->nodeId               = nodeId;
        data->endpointId           = endpointId;
        data->clusterId            = clusterId;
        data->attributeId          = attributeId;
        DeviceLayer::PlatformMgr().ScheduleWork(InteractionModelHelperWorkerFunction, reinterpret_cast<intptr_t>(data));
        return CHIP_NO_ERROR;
    }

    // CHIP_ERROR write(NodeId nodeId, EndpointId endpointId, ClusterId clusterId, AttributeId commandId, const T value);

private:
    static void InteractionModelHelperWorkerFunction(intptr_t context);
};
enum CommandType
{
    subscribe,
    read,
    write,
    command,
};

}; // namespace chip