#pragma once

#include "Subscription.h"
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
class SubscriptionManager
{
public:
    SubscriptionManager() :
        mOnConnectedCallbackSubscribeRequest(onConnectedCallbackSubscribeRequest, this),
        mOnFailureCallbackSubscribeRequest(onFailureCallbackSubscribeRequest, this)
    {
        auto & server       = chip::Server::GetInstance();
        mFabricTable        = &server.GetFabricTable();
        mCASESessionManager = server.GetCASESessionManager();
    };
    /*CHIP_ERROR RegisterSubscription(chip::FabricId fabricId, chip::app::ConcreteAttributePath attributesPath,
                                    chip::SubscriptionId * subscriptionId); // subscriptionId is out param
    CHIP_ERROR RegisterSubscription(chip::FabricId fabricId, chip::app::ConcreteEventPath eventPath,
                                    chip::SubscriptionId * subscriptionId); // subscriptionId is out param*/
    CHIP_ERROR RegisterSubscription(SubscribeCommandData * data);
    std::vector<chip::SubscriptionId> GetActiveSubscriptions();
    CHIP_ERROR UnregisterSubscription(chip::SubscriptionId subscriptionId);

    static SubscriptionManager & GetInstance() { return sSubscriptionManager; }

private:
    static SubscriptionManager sSubscriptionManager;

    static void onConnectedCallbackSubscribeRequest(void * context, chip::OperationalDeviceProxy * peer_device);
    static void onFailureCallbackSubscribeRequest(void * context, chip::PeerId peerId, CHIP_ERROR error);

    chip::FabricTable * mFabricTable               = nullptr;
    chip::CASESessionManager * mCASESessionManager = nullptr;
    chip::Callback::Callback<chip::OnDeviceConnected> mOnConnectedCallbackSubscribeRequest;
    chip::Callback::Callback<chip::OnDeviceConnectionFailure> mOnFailureCallbackSubscribeRequest;

    CHIP_ERROR SendSubscribeRequest(SubscribeCommandData * data);
};
} // namespace chip