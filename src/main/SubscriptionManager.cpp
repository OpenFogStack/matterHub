#include "SubscriptionManager.h"
#include "ClusterCommands.h"
#include "SubscribeCommands.h"
#include "app/CommandSender.h"
#include "app/clusters/bindings/BindingManager.h"
#include "app/server/Server.h"
#include "controller/InvokeInteraction.h"
#include "platform/CHIPDeviceLayer.h"
#include <app/clusters/bindings/bindings.h>
#include <lib/support/CodeUtils.h>

namespace chip {
SubscriptionManager SubscriptionManager::sSubscriptionManager;
CHIP_ERROR SubscriptionManager::RegisterSubscription(shell::SubscribeCommandData * data)
{
    // Send subscribe request
    SubscriptionManager::SendSubscribeRequest(data);
    return CHIP_NO_ERROR;
}
namespace {

chip::PeerId PeerIdForNode(chip::FabricTable * fabricTable, chip::FabricIndex fabric, chip::NodeId node)
{
    chip::FabricInfo * fabricInfo = fabricTable->FindFabricWithIndex(fabric);
    if (fabricInfo == nullptr)
    {
        return chip::PeerId();
    }
    return fabricInfo->GetPeerIdForNode(node);
}
} // namespace

CHIP_ERROR SubscriptionManager::SendSubscribeRequest(shell::SubscribeCommandData * data)
{

    PeerId peer = PeerIdForNode(mFabricTable, data->fabricId, data->nodeId);
    if (peer.GetNodeId() == kUndefinedNodeId)
    {
        ChipLogError(NotSpecified, "ClusterCommandWorkerFunction - Unable to find the mentioned Peer");
        return CHIP_ERROR_INVALID_ARGUMENT;
    }
    ESP_LOGI("Subscribe", "SubscribeSubscribeCommandHandler");
    ESP_LOGI("Subscribe", " - EndPoint ID: '0x%02x'", data->endpointId);
    ESP_LOGI("Subscribe", " - Cluster ID: '0x%02x'", data->clusterId);
    ESP_LOGI("Subscribe", " - Attribute ID: '0x%02x'", data->attributeId);
    ESP_LOGI("Subscribe", " - minInterval: '0x%02x'", data->minInterval);
    ESP_LOGI("Subscribe", " - maxInterval: '0x%02x'", data->maxInterval);

    mCASESessionManager->FindOrEstablishSession(peer, &data->mOnConnectedCallback, &data->mOnConnectionFailureCallback);
    ChipLogError(NotSpecified, "ClusterCommandWorkerFunction - Registration Done");
    return CHIP_NO_ERROR;
}

} // namespace chip
