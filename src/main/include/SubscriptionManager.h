#pragma once

#include "core/CHIPError.h"
#include "Subscription.h"
#include "core/DataModelTypes.h"
#include <vector>

/*

*/
class SubscriptionManager{
    public:
        static SubscriptionManager* GetInstance();
        CHIP_ERROR RegisterSubscription(chip::FabricId fabricId, chip::app::ConcreteAttributePath attributesPath, std::out_ptr<chip::SubscriptionId> subscriptionId); //subscriptionId is out param
        CHIP_ERROR RegisterSubscription(chip::FabricId fabricId, chip::app::ConcreteEventPath eventPath, std::out_ptr<chip::SubscriptionId> subscriptionId); //subscriptionId is out param
        std::vector<chip::SubscriptionId> GetActiveSubscriptions();
        CHIP_ERROR UnregisterSubscription(chip::SubscriptionId subscriptionId);
    private:
        SubscriptionManager(){

        };
};