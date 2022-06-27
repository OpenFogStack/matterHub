/*
 *    Published under MIT License
 *    Copyright (c) 2022 OpenFogStack
 *    Based on code written by Project CHIP Authors, which was published under Apache License, Version 2.0 (c) 2022
 */
#pragma once
#include <app/OperationalDeviceProxy.h>

namespace shell {

struct BaseCommandData
{
    BaseCommandData(chip::OnDeviceConnected callback, void* context):onConnectedCallback(callback, context){};
    chip::FabricId fabricId;
    chip::NodeId nodeId;
    chip::Callback::Callback<chip::OnDeviceConnected> onConnectedCallback;
};
} // namespace shell