#pragma once
#include "BaseCommand.h"
#include "Subscription.h"
#include "platform/CHIPDeviceLayer.h"
#include <InteractionModel.h>
#include <app/OperationalDeviceProxy.h>

namespace chip {

struct SubscribeCommandData : shell::BaseCommandData
{
    chip::EndpointId endpointId;
    chip::ClusterId clusterId;
    chip::AttributeId attributeId;
    SubscriptionCallback callback;
    uint16_t minInterval;
    uint16_t maxInterval;
    SubscribeCommandData();
};

struct ReadCommandData : shell::BaseCommandData
{
    chip::EndpointId endpointId;
    chip::ClusterId clusterId;
    chip::AttributeId attributeId;
    SubscriptionCallback callback;
    ReadCommandData();
};

template <typename T>
struct WriteCommandData : shell::BaseCommandData
{
    chip::EndpointId endpointId;
    chip::ClusterId clusterId;
    chip::AttributeId attributeId;
    T & value;
    WriteCommandData(T & value);
};

struct CommandCommandData : shell::BaseCommandData
{
    chip::EndpointId endpointId;
    chip::ClusterId clusterId;
    chip::CommandId commandId;
    CommandCommandData();
};
} // namespace chip