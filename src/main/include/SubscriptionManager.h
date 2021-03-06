#pragma once

#include "Subscription.h"
#include "core/CHIPError.h"
#include "core/DataModelTypes.h"
#include <app/OperationalDeviceProxy.h>

#include "SubscribeCommands.h"
#include "app/server/Server.h"
#include "platform/CHIPDeviceLayer.h"
#include <app/clusters/bindings/bindings.h>
#include <lib/support/CodeUtils.h>
#include <vector>

/*

*/
namespace chip {
class SubscriptionManager
{
public:
    SubscriptionManager()
    {
        auto & server       = chip::Server::GetInstance();
        mFabricTable        = &server.GetFabricTable();
        mCASESessionManager = server.GetCASESessionManager();
    };
    CHIP_ERROR RegisterSubscription(shell::SubscribeCommandData * data);
    std::vector<chip::SubscriptionId> GetActiveSubscriptions();
    CHIP_ERROR UnregisterSubscription(chip::SubscriptionId subscriptionId);

    static SubscriptionManager & GetInstance() { return sSubscriptionManager; }
    static void cleanup(Subscription * ptr) { Platform::Delete(ptr); }
    std::vector<std::unique_ptr<Subscription, decltype(&cleanup)>> mSubscriptions;

private:
    static SubscriptionManager sSubscriptionManager;

    chip::FabricTable * mFabricTable               = nullptr;
    chip::CASESessionManager * mCASESessionManager = nullptr;

    CHIP_ERROR SendSubscribeRequest(shell::SubscribeCommandData * data);
};
} // namespace chip