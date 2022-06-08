/*
 *    Published under MIT License
 *    Copyright (c) 2022 OpenFogStack
 *    Based on code written by Project CHIP Authors, which was published under Apache License, Version 2.0 (c) 2022
 */
#pragma once

#include "app/server/Server.h"
#include <app/OperationalDeviceProxy.h>
namespace shell {

void RegisterSubscribeCommands();
void SubscribeCommandWorkerFunction(intptr_t context);
struct SubscribeCommandData
{
    chip::FabricId fabricId;
    chip::NodeId nodeId;
    chip::EndpointId endpointId;
    chip::ClusterId clusterId;
    chip::AttributeId attributeId;
    uint16_t minInterval;
    uint16_t maxInterval;
};
} // namespace shell