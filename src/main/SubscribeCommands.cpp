
/*
 *    Published under MIT License
 *    Copyright (c) 2022 OpenFogStack
 *    Based on code written by Project CHIP Authors, which was published under Apache License, Version 2.0 (c) 2022
 */
#include "SubscriptionManager.h"
#include "app/CommandSender.h"
#include "app/clusters/bindings/BindingManager.h"
#include "app/server/Server.h"
#include "controller/InvokeInteraction.h"
#include "platform/CHIPDeviceLayer.h"
#include <app/clusters/bindings/bindings.h>
#include <lib/support/CodeUtils.h>

#include "lib/shell/Engine.h"
#include "lib/shell/commands/Help.h"

using namespace chip;
using namespace chip::app;

using Shell::Engine;
using Shell::shell_command_t;
using Shell::streamer_get;
using Shell::streamer_printf;

static const char * TAG = "Subscribe Commands";
namespace shell {
Engine sShellSubscribeSubCommands;

/********************************************************
 * Subscribe shell functions
 *********************************************************/

CHIP_ERROR SubscribeHelpHandler(int argc, char ** argv)
{
    sShellSubscribeSubCommands.ForEachCommand(Shell::PrintCommandHelp, nullptr);
    return CHIP_NO_ERROR;
}

CHIP_ERROR SubscribeCommandHandler(int argc, char ** argv)
{
    if (argc == 0)
    {
        return SubscribeHelpHandler(argc, argv);
    }

    return sShellSubscribeSubCommands.ExecCommand(argc, argv);
}

void SubscribeWorkerFunction(intptr_t context)
{
    VerifyOrReturn(context != 0, ChipLogError(NotSpecified, "SubscribeWorkerFunction - Invalid work data"));

    SubscribeCommandData * data = reinterpret_cast<SubscribeCommandData *>(context);

    SubscriptionManager::GetInstance().RegisterSubscription(data);

    Platform::Delete(data);
}

CHIP_ERROR SubscribeSubscribeCommandHandler(int argc, char ** argv)
{
    SubscribeCommandData * entry = Platform::New<SubscribeCommandData>();
    if (argc != 7)
    {
        return SubscribeHelpHandler(argc, argv);
    }
    entry->fabricId    = atoi(argv[0]);
    entry->nodeId      = atoi(argv[1]);
    entry->endpointId  = atoi(argv[2]);
    entry->clusterId   = atoi(argv[3]);
    entry->attributeId = atoi(argv[4]);
    entry->minInterval = atoi(argv[5]);
    entry->maxInterval = atoi(argv[6]);

    ESP_LOGI("Subscribe", "SubscribeSubscribeCommandHandler");
    ESP_LOGI("Subscribe", " - EndPoint ID: '0x%02x'", entry->endpointId);
    ESP_LOGI("Subscribe", " - Cluster ID: '0x%02x'", entry->clusterId);
    ESP_LOGI("Subscribe", " - Attribute ID: '0x%02x'", entry->attributeId);
    ESP_LOGI("Subscribe", " - minInterval: '0x%02x'", entry->minInterval);
    ESP_LOGI("Subscribe", " - maxInterval: '0x%02x'", entry->maxInterval);

    DeviceLayer::PlatformMgr().ScheduleWork(SubscribeWorkerFunction, reinterpret_cast<intptr_t>(entry));
    return CHIP_NO_ERROR;
}

CHIP_ERROR UnsubscribeCommandHandler(int argc, char ** argv)
{
    if (argc == 0)
    {
        return SubscribeHelpHandler(argc, argv);
    }

    return sShellSubscribeSubCommands.ExecCommand(argc, argv);
}

void RegisterSubscribeCommands()
{
    static const shell_command_t sSubscribeSubCommands[] = {
        { &SubscribeHelpHandler, "help", "Usage: subscribe <subcommand>" },
        { &SubscribeSubscribeCommandHandler, "subscribe",
          "Usage: subscribe <fabric id> <node id> <endpoint id> <cluster id> <attribute id> <min_interval> <max_interval>" },
        { &UnsubscribeCommandHandler, "unsubscribe", "Usage: unsubscribe <subscription id>" },
    };
    sShellSubscribeSubCommands.RegisterCommands(sSubscribeSubCommands, ArraySize(sSubscribeSubCommands));

    static const shell_command_t sSubscribeCommand = { &SubscribeCommandHandler, "subscribe",
                                                       "Subscribtion commands. Usage: subscribe <subcommand>" };

    Engine::Root().RegisterCommands(&sSubscribeCommand, 1);
}

void onFailureCallbackSubscribeRequest(void * context, chip::PeerId peerId, CHIP_ERROR error)
{
    auto & server                                 = chip::Server::GetInstance();
    chip::CASESessionManager * CASESessionManager = server.GetCASESessionManager();
    // Simply release the entry, the connection will be re-established as needed.
    ChipLogError(NotSpecified, "Failed to establish connection to node 0x" ChipLogFormatX64, ChipLogValueX64(peerId.GetNodeId()));
    CASESessionManager->ReleaseSession(peerId);
}

void onConnectedCallbackSubscribeRequest(void * context, chip::OperationalDeviceProxy * peer_device)
{
    ChipLogError(NotSpecified, "Got here!!");

    SubscribeCommandData * data = reinterpret_cast<shell::SubscribeCommandData *>(context);
    ESP_LOGI("Subscribe", "SubscribeSubscribeCommandHandler");
    ESP_LOGI("Subscribe", " - EndPoint ID: '0x%02x'", data->endpointId);
    ESP_LOGI("Subscribe", " - Cluster ID: '0x%02x'", data->clusterId);
    ESP_LOGI("Subscribe", " - Attribute ID: '0x%02x'", data->attributeId);
    ESP_LOGI("Subscribe", " - minInterval: '0x%02x'", data->minInterval);
    ESP_LOGI("Subscribe", " - maxInterval: '0x%02x'", data->maxInterval);
    Subscription * sub = Platform::New<Subscription>(peer_device, data->endpointId, data->clusterId, data->attributeId,
                                                     data->minInterval, data->maxInterval);

    ESP_LOGI("Subscribe", "SubscribeSubscribeCommandHandler");
    ESP_LOGI("Subscribe", " - EndPoint ID: '0x%02x'", sub->mEndpointId[0]);
    ESP_LOGI("Subscribe", " - Cluster ID: '0x%02x'", sub->mClusterId[0]);
    ESP_LOGI("Subscribe", " - Attribute ID: '0x%02x'", sub->mAttributeId[0]);
    ESP_LOGI("Subscribe", " - minInterval: '0x%02x'", sub->mMinInterval);
    ESP_LOGI("Subscribe", " - maxInterval: '0x%02x'", sub->mMaxInterval);
    CHIP_ERROR error = sub->DoSubscribe();
    if (error == CHIP_NO_ERROR)
    {
        SubscriptionManager::GetInstance().mSubscriptions.emplace_back(sub, &SubscriptionManager::cleanup);
    }
    else
    {
        ChipLogError(NotSpecified, "Got error while subscribing: %" CHIP_ERROR_FORMAT, error.Format());
        SubscriptionManager::cleanup(sub);
        // TODO remove cleanup, use platform::Delete
    }

    // Platform::Delete(data);
}
SubscribeCommandData::SubscribeCommandData() :
    mOnConnectedCallback(onConnectedCallbackSubscribeRequest, (void *) this),
    mOnConnectionFailureCallback(onFailureCallbackSubscribeRequest, (void *) this)
{}
} // namespace shell