#pragma once
#include "BaseCommand.h"

#include "core/CHIPError.h"
#include "core/DataModelTypes.h"
#include <app/OperationalDeviceProxy.h>

#include "app/server/Server.h"
#include "platform/CHIPDeviceLayer.h"
#include <app/clusters/bindings/bindings.h>
#include <lib/support/CodeUtils.h>
#include <vector>

/*

*/
namespace chip {

void onConnectionRequestFailure(void * context, chip::PeerId peerId, CHIP_ERROR error);

class ConnectionHelper
{
public:
    ConnectionHelper() : mOnConnectionFailureCallback(onConnectionRequestFailure, (void *) this)
    {
        auto & server       = chip::Server::GetInstance();
        mFabricTable        = &server.GetFabricTable();
        mCASESessionManager = server.GetCASESessionManager();
    };

    static ConnectionHelper & GetInstance()
    {
        static ConnectionHelper sConnectionHelper;
        return sConnectionHelper;
    }
    CHIP_ERROR RequestConnection(shell::BaseCommandData * data);

private:
    chip::Callback::Callback<chip::OnDeviceConnectionFailure> mOnConnectionFailureCallback;
    chip::FabricTable * mFabricTable               = nullptr;
    chip::CASESessionManager * mCASESessionManager = nullptr;
};
} // namespace chip