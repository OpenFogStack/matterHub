/*
 *    Published under MIT License
 *    Copyright (c) 2022 OpenFogStack
 *    Based on code written by Project CHIP Authors, which was published under Apache License, Version 2.0 (c) 2022
 */

#include "ConnectionHelper.h"
#include "InteractionModelHelper.h"
#include "app/CommandSender.h"
#include "app/clusters/bindings/BindingManager.h"
#include "app/server/Server.h"
#include "controller/InvokeInteraction.h"
#include "platform/CHIPDeviceLayer.h"
#include <app/clusters/bindings/bindings.h>
#include <lib/support/CodeUtils.h>

namespace chip {
void onConnectionRequestFailure(void * context, chip::PeerId peerId, CHIP_ERROR error)
{

    shell::BaseCommandData * data                 = reinterpret_cast<shell::BaseCommandData *>(context);
    auto & server                                 = chip::Server::GetInstance();
    chip::CASESessionManager * CASESessionManager = server.GetCASESessionManager();
    // Simply release the entry, the connection will be re-established as needed.
    ChipLogError(NotSpecified, "Failed to establish connection to node 0x" ChipLogFormatX64, ChipLogValueX64(peerId.GetNodeId()));
    CASESessionManager->ReleaseSession(peerId);
    Platform::Delete(data);
}

namespace {

chip::PeerId PeerIdForNode(chip::FabricTable * fabricTable, chip::FabricIndex fabric, chip::NodeId node)
{
    chip::FabricInfo const * const fabricInfo = fabricTable->FindFabricWithIndex(fabric);
    if (fabricInfo == nullptr)
    {
        return chip::PeerId();
    }
    return fabricInfo->GetPeerIdForNode(node);
}
} // namespace

CHIP_ERROR ConnectionHelper::RequestConnection(shell::BaseCommandData * data)
{

    PeerId peer = PeerIdForNode(mFabricTable, data->fabricId, data->nodeId);
    if (peer.GetNodeId() == kUndefinedNodeId)
    {
        ChipLogError(NotSpecified, "ConnectionHelper - Unable to find the mentioned Peer");
        return CHIP_ERROR_INVALID_ARGUMENT;
    }

    mCASESessionManager->FindOrEstablishSession(peer, &data->onConnectedCallback, &mOnConnectionFailureCallback);
    ChipLogError(NotSpecified, "ConnectionHelper - Registration Done");
    return CHIP_NO_ERROR;
}

} // namespace chip
