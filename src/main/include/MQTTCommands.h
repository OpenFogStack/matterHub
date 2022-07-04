/*
 *    Published under MIT License
 *    Copyright (c) 2022 OpenFogStack
 *    Based on code written by Project CHIP Authors, which was published under Apache License, Version 2.0 (c) 2022
 */
#pragma once

#include <app/OperationalDeviceProxy.h>
namespace shell {

void RegisterMQTTCommands();
void MQTTCommandWorkerFunction(intptr_t context);
enum MQTTCommandTask
{
    publish,
    subscribe,
    unsubscribe,
};
struct MQTTCommandData
{
    enum MQTTCommandTask task;
    char * topic;
    char * data;
};
} // namespace shell
