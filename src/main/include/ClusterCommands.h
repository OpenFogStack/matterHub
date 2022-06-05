/*
 *    Published under MIT License
 *    Copyright (c) 2022 OpenFogStack
 *    Based on code written by Project CHIP Authors, which was published under Apache License, Version 2.0 (c) 2022
 */
#pragma once

#include <app/OperationalDeviceProxy.h>
namespace shell
{

    void RegisterClusterCommands();
    void ClusterCommandWorkerFunction(intptr_t context);
    struct ClusterCommandData
    {
        ClusterCommandData();
        chip::FabricId fabricId;
        chip::NodeId nodeId;
        chip::EndpointId endpointId;
        chip::CommandId commandId;
        chip::ClusterId clusterId;
        chip::Callback::Callback<chip::OnDeviceConnected> mOnConnectedCallback;
        chip::Callback::Callback<chip::OnDeviceConnectionFailure> mOnConnectionFailureCallback;
    };
}