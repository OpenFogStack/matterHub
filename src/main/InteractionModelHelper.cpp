#include "InteractionModelHelper.h"
#include "BaseCommand.h"

// todo
#include "app/CommandSender.h"
#include "app/clusters/bindings/BindingManager.h"
#include "app/server/Server.h"
#include "controller/InvokeInteraction.h"
#include "controller/ReadInteraction.h"
#include "platform/CHIPDeviceLayer.h"
#include <app/clusters/bindings/bindings.h>
#include <lib/support/CodeUtils.h>

namespace chip {
CHIP_ERROR InteractionModelHelper::subscribe(NodeId nodeId, EndpointId endpointId, ClusterId clusterId, AttributeId attributeId,
                                             uint16_t minInterval, uint16_t maxInterval, SubscriptionCallback callback)
{
    SubscribeCommandData * data = Platform::New<SubscribeCommandData>();

    ConnectionHelper::GetInstance().RequestConnection(data);
    return CHIP_NO_ERROR;
}
CHIP_ERROR InteractionModelHelper::command(NodeId nodeId, EndpointId endpointId, ClusterId clusterId, CommandId commandId)
{
    CommandCommandData * data = Platform::New<CommandCommandData>();
    data->fabricId            = 1;
    data->nodeId              = nodeId;
    data->endpointId          = endpointId;
    data->commandId           = commandId;
    ConnectionHelper::GetInstance().RequestConnection(data);
    return CHIP_NO_ERROR;
}
CHIP_ERROR InteractionModelHelper::read(NodeId nodeId, EndpointId endpointId, ClusterId clusterId, AttributeId attributeId)
{

    return CHIP_NO_ERROR;
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
} // namespace chip