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
    // TODO build some datastructure to store Subscriptions

    // Send subscribe request
    SubscriptionManager::SendSubscribeRequest(data);
    // Receive report data?!

    // send status response ?!
    return CHIP_NO_ERROR;
}

CHIP_ERROR SubscriptionManager::SendSubscribeRequest(SubscribeCommandData * data)
{

    /*PeerId peer = PeerIdForNode(mfabricTable, data->fabricId, data->nodeId);
    if (peer.GetNodeId() == kUndefinedNodeId)
    {
        ChipLogError(NotSpecified, "ClusterCommandWorkerFunction - Unable to find the mentioned Peer");
        return;
    }

    mCASESessionManager->FindOrEstablishSession(peer, onConnectedCallbackSubscribeRequest, &mOnConnectionFailureCallback);*/
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
    SubscribeCommandData * data = reinterpret_cast<SubscribeCommandData *>(context);

    auto onSuccess = [data](const chip::app::ConcreteCommandPath & commandPath, const chip::app::StatusIB & status,
                            const auto & dataResponse) {
        ChipLogProgress(NotSpecified, "Subscribe Request has been sent");
        chip::Platform::Delete(data);
    };

    auto onFailure = [data](CHIP_ERROR error) {
        ChipLogError(NotSpecified, "Subscribe Request has failed: %" CHIP_ERROR_FORMAT, error.Format());
        chip::Platform::Delete(data);
    };
}
} // namespace chip