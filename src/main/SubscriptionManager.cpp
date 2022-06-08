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
CHIP_ERROR SubscriptionManager::RegisterSubscription(SubscribeCommandData * data)
{
    if(mCurrentSubscription.HasValue()) {
        return CHIP_ERROR_BUSY;
    }
    mCurrentSubscription.SetValue(data);
    // Send subscribe request
    SubscriptionManager::SendSubscribeRequest(data);
    // Receive report manager?!

    // send status response ?!
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

CHIP_ERROR SubscriptionManager::SendSubscribeRequest(SubscribeCommandData * data)
{

    PeerId peer = PeerIdForNode(mFabricTable, data->fabricId, data->nodeId);
    if (peer.GetNodeId() == kUndefinedNodeId)
    {
        ChipLogError(NotSpecified, "ClusterCommandWorkerFunction - Unable to find the mentioned Peer");
        return CHIP_ERROR_INVALID_ARGUMENT;
    }

    mCASESessionManager->FindOrEstablishSession(peer, &mOnConnectedCallbackSubscribeRequest, &mOnFailureCallbackSubscribeRequest);
    ChipLogError(NotSpecified, "ClusterCommandWorkerFunction - Registration Done");
    return CHIP_NO_ERROR;
}

void SubscriptionManager::onFailureCallbackSubscribeRequest(void * context, chip::PeerId peerId, CHIP_ERROR error)
{
    auto & server                                 = chip::Server::GetInstance();
    chip::CASESessionManager * CASESessionManager = server.GetCASESessionManager();
    // Simply release the entry, the connection will be re-established as needed.
    ChipLogError(NotSpecified, "Failed to establish connection to node 0x" ChipLogFormatX64, ChipLogValueX64(peerId.GetNodeId()));
    CASESessionManager->ReleaseSession(peerId);
}

void SubscriptionManager::onConnectedCallbackSubscribeRequest(void * context, chip::OperationalDeviceProxy * peer_device)
{
    ChipLogError(NotSpecified, "Got here!!");
    SubscriptionManager * manager = reinterpret_cast<SubscriptionManager *>(context);
    assert(manager->mCurrentSubscription.HasValue());
    SubscribeCommandData * data = manager->mCurrentSubscription.Value();
    Subscription * sub = Platform::New<Subscription>(peer_device, data->endpointId, data->clusterId, data->attributeId,
                                                     data->minInterval, data->maxInterval);

    //TODO Start subscription
    manager->mSubscriptions.emplace_back(sub, &cleanup);
    manager->mCurrentSubscription.ClearValue();
    Platform::Delete(data);
}
} // namespace chip