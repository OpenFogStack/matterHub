#pragma once
#include "ConnectionHelper.h"
#include <InteractionModel.h>
#include <app/OperationalDeviceProxy.h>

namespace chip {
typedef void (*SubscriptionCallback)(const chip::app::ConcreteDataAttributePath &, chip::TLV::TLVReader *);
class InteractionModelHelper
{
public:
    InteractionModelHelper();

    static CHIP_ERROR subscribe(NodeId nodeId, EndpointId endpointId, ClusterId clusterId, AttributeId attributeId,
                                uint16_t minInterval, uint16_t maxInterval, SubscriptionCallback callback);
    static CHIP_ERROR command(NodeId nodeId, EndpointId endpointId, ClusterId clusterId, CommandId commandId);

    static CHIP_ERROR read(NodeId nodeId, EndpointId endpointId, ClusterId clusterId, AttributeId commandId);

    // CHIP_ERROR write(NodeId nodeId, EndpointId endpointId, ClusterId clusterId, AttributeId commandId, const T value);

private:
};
enum CommandType
{
    subscribe,
    read,
    write,
    command,
};

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

struct WriteCommandData : shell::BaseCommandData
{
    chip::EndpointId endpointId;
    chip::ClusterId clusterId;
    chip::AttributeId attributeId;
    WriteCommandData();
};

struct CommandCommandData : shell::BaseCommandData
{
    chip::EndpointId endpointId;
    chip::ClusterId clusterId;
    chip::CommandId commandId;
    CommandCommandData();
};
}; // namespace chip