#include "InteractionModelHelper.h"
#include "BaseCommand.h"
#include "Subscription.h"

// todo
#include "BaseCommand.h"
#include "app/CommandSender.h"
#include "app/clusters/bindings/BindingManager.h"
#include "app/server/Server.h"
#include "controller/InvokeInteraction.h"
#include "controller/ReadInteraction.h"
#include "platform/CHIPDeviceLayer.h"
#include <app-common/zap-generated/cluster-objects.h>
#include <app/clusters/bindings/bindings.h>
#include <controller-clusters/zap-generated/CHIPClusters.h>
#include <lib/support/CodeUtils.h>

namespace chip {
void InteractionModelHelperWorkerFunction(intptr_t context)
{
    VerifyOrReturn(context != 0, ChipLogError(NotSpecified, "InteractionModelHelperWorkerFunction - Invalid work data"));

    shell::BaseCommandData * data = reinterpret_cast<shell::BaseCommandData *>(context);
    ConnectionHelper::GetInstance().RequestConnection(data);
}
CHIP_ERROR InteractionModelHelper::subscribe(NodeId nodeId, EndpointId endpointId, ClusterId clusterId, AttributeId attributeId,
                                             uint16_t minInterval, uint16_t maxInterval, SubscriptionCallback callback)
{
    SubscribeCommandData * data = Platform::New<SubscribeCommandData>();
    data->fabricId              = 1;
    data->nodeId                = nodeId;
    data->endpointId            = endpointId;
    data->clusterId             = clusterId;
    data->attributeId           = attributeId;
    data->minInterval           = minInterval;
    data->maxInterval           = maxInterval;
    data->callback              = callback;
    DeviceLayer::PlatformMgr().ScheduleWork(InteractionModelHelperWorkerFunction, reinterpret_cast<intptr_t>(data));
    return CHIP_NO_ERROR;
}
CHIP_ERROR InteractionModelHelper::command(NodeId nodeId, EndpointId endpointId, ClusterId clusterId, CommandId commandId)
{
    CommandCommandData * data = Platform::New<CommandCommandData>();
    data->fabricId            = 1;
    data->nodeId              = nodeId;
    data->endpointId          = endpointId;
    data->clusterId           = clusterId;
    data->commandId           = commandId;
    DeviceLayer::PlatformMgr().ScheduleWork(InteractionModelHelperWorkerFunction, reinterpret_cast<intptr_t>(data));
    return CHIP_NO_ERROR;
}
CHIP_ERROR InteractionModelHelper::read(NodeId nodeId, EndpointId endpointId, ClusterId clusterId, AttributeId attributeId,
                                        SubscriptionCallback callback)
{
    ReadCommandData * data = Platform::New<ReadCommandData>();
    data->fabricId         = 1;
    data->nodeId           = nodeId;
    data->endpointId       = endpointId;
    data->clusterId        = clusterId;
    data->attributeId      = attributeId;
    data->callback         = callback;
    DeviceLayer::PlatformMgr().ScheduleWork(InteractionModelHelperWorkerFunction, reinterpret_cast<intptr_t>(data));
    return CHIP_NO_ERROR;
}

void onConnectedCallbackRead(void * context, chip::OperationalDeviceProxy * peer_device)
{
    ReadCommandData * data = reinterpret_cast<chip::ReadCommandData *>(context);

    Subscription * sub =
        Platform::New<Subscription>(peer_device, data->endpointId, data->clusterId, data->attributeId, data->callback);

    CHIP_ERROR error = sub->Read();
    Platform::Delete(data);
}

void onConnectedCallbackSubscribe(void * context, chip::OperationalDeviceProxy * peer_device)
{
    SubscribeCommandData * data = reinterpret_cast<chip::SubscribeCommandData *>(context);

    Subscription * sub =
        Platform::New<Subscription>(peer_device, data->endpointId, data->clusterId, data->attributeId, data->callback);

    CHIP_ERROR error = sub->DoSubscribe();
    Platform::Delete(data);
}
void onConnectedCallbackCommand(void * context, chip::OperationalDeviceProxy * peer_device)
{
    CommandCommandData * data = reinterpret_cast<chip::CommandCommandData *>(context);
    ESP_LOGI("Interaction Model Helper", "Requested Command!");

    auto onSuccess = [](const chip::app::ConcreteCommandPath & commandPath, const chip::app::StatusIB & status,
                        const auto & dataResponse) { ChipLogProgress(NotSpecified, "OnOff command succeeds"); };

    auto onFailure = [](CHIP_ERROR error) {
        ChipLogError(NotSpecified, "OnOff command failed: %" CHIP_ERROR_FORMAT, error.Format());
    };

    switch (data->commandId)
    {
    case chip::app::Clusters::OnOff::Commands::Off::Id:
        chip::app::Clusters::OnOff::Commands::Off::Type offCommand;
        chip::Controller::InvokeCommandRequest(peer_device->GetExchangeManager(), peer_device->GetSecureSession().Value(),
                                               data->endpointId, offCommand, onSuccess, onFailure);
        break;

    case chip::app::Clusters::OnOff::Commands::On::Id:
        chip::app::Clusters::OnOff::Commands::On::Type onCommand;
        chip::Controller::InvokeCommandRequest(peer_device->GetExchangeManager(), peer_device->GetSecureSession().Value(),
                                               data->endpointId, onCommand, onSuccess, onFailure);
        break;

    case chip::app::Clusters::OnOff::Commands::Toggle::Id:
        chip::app::Clusters::OnOff::Commands::Toggle::Type toggleCommand;
        chip::Controller::InvokeCommandRequest(peer_device->GetExchangeManager(), peer_device->GetSecureSession().Value(),
                                               data->endpointId, toggleCommand, onSuccess, onFailure);
        break;
    default:
        ESP_LOGE("Interaction Model Helper", "Unsupported CommandId: %d", data->commandId);
        break;
    }
}
CommandCommandData::CommandCommandData() : BaseCommandData(onConnectedCallbackCommand, (void *) this) {}
ReadCommandData::ReadCommandData() : BaseCommandData(onConnectedCallbackRead, (void *) this) {}
SubscribeCommandData::SubscribeCommandData() : BaseCommandData(onConnectedCallbackSubscribe, (void *) this) {}

} // namespace chip