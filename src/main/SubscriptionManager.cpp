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
CHIP_ERROR SubscriptionManager::RegisterSubscription(SubscribeCommandData * manager)
{
    // TODO build some datastructure to store Subscriptions

    // Send subscribe request
    SubscriptionManager::SendSubscribeRequest(manager);
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

static void SubscriptionManager::onFailureCallbackSubscribeRequest(void * context, chip::PeerId peerId, CHIP_ERROR error)
{
    auto & server                                 = chip::Server::GetInstance();
    chip::CASESessionManager * CASESessionManager = server.GetCASESessionManager();
    // Simply release the entry, the connection will be re-established as needed.
    ChipLogError(NotSpecified, "Failed to establish connection to node 0x" ChipLogFormatX64, ChipLogValueX64(peerId.GetNodeId()));
    CASESessionManager->ReleaseSession(peerId);
}

static void SubscriptionManager::onConnectedCallbackSubscribeRequest(void * context, chip::OperationalDeviceProxy * peer_device)
{
    ChipLogError(NotSpecified, "Got here!!");
    SubscriptionManager * manager = reinterpret_cast<SubscriptionManager *>(context);
    auto & sub                    = manager->mSubscriptions.emplace_back();
    sub.SubscribeAttribute();
}
} // namespace chip