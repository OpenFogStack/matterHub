/*
 *
 *    Copyright (c) 2022 Project CHIP Authors
 *    All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#pragma once

#include "app-common/zap-generated/ids/Clusters.h"
#include "app-common/zap-generated/ids/Commands.h"
#include "app/util/attribute-metadata.h"
#include "lib/core/CHIPError.h"
#include <app/BufferedReadCallback.h>
#include <app/ChunkedWriteCallback.h>
#include <app/CommandSender.h>
#include <app/DeviceProxy.h>
#include <app/ReadClient.h>
#include <app/WriteClient.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/UnitTestUtils.h>


CHIP_ERROR InitBindingHandler();
void SwitchWorkerFunction(intptr_t context);
void BindingWorkerFunction(intptr_t context);

struct BindingCommandData
{
    chip::EndpointId localEndpointId = 1;
    chip::CommandId commandId;
    chip::ClusterId clusterId;
    bool isGroup = false;
};

struct subscribeData {
    const char * identity; 
    chip::EndpointId endpointId;
    chip::ClusterId clusterId;
    chip::AttributeId attributeId;
    uint16_t minInterval;
    uint16_t maxInterval;
};