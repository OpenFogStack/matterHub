/*
 *    Published under MIT License
 *    Copyright (c) 2022 OpenFogStack
 *    Based on code written by Project CHIP Authors, which was published under Apache License, Version 2.0 (c) 2022
 */
#include "MQTTCommands.h"
#include "MQTTManager.h"
#include "app/server/Server.h"
#include <lib/support/CodeUtils.h>

#if CONFIG_ENABLE_CHIP_SHELL
#include "lib/shell/Engine.h"
#include "lib/shell/commands/Help.h"
#endif // ENABLE_CHIP_SHELL

using namespace chip;
using namespace chip::app;

#if CONFIG_ENABLE_CHIP_SHELL
using Shell::Engine;
using Shell::shell_command_t;
using Shell::streamer_get;
using Shell::streamer_printf;

Engine sShellMQTTSubCommands;

#endif // defined(ENABLE_CHIP_SHELL)

static const char * TAG = "MQTT Commands";
namespace shell {

#ifdef CONFIG_ENABLE_CHIP_SHELL

/********************************************************
 * MQTT shell functions
 *********************************************************/

CHIP_ERROR MQTTHelpHandler(int argc, char ** argv)
{
    sShellMQTTSubCommands.ForEachCommand(Shell::PrintCommandHelp, nullptr);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MQTTCommandHandler(int argc, char ** argv)
{
    if (argc == 0)
    {
        return MQTTHelpHandler(argc, argv);
    }

    return sShellMQTTSubCommands.ExecCommand(argc, argv);
}

/********************************************************
 * MQTT sub and pub shell functions
 *********************************************************/

CHIP_ERROR MQTTSubCommandHelpHandler(int argc, char ** argv)
{
    sShellMQTTSubCommands.ForEachCommand(Shell::PrintCommandHelp, nullptr);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MQTTSubHandler(int argc, char ** argv)
{
    if (argc != 1)
    {
        return MQTTSubCommandHelpHandler(argc, argv);
    }
    MQTTCommandData * data = Platform::New<MQTTCommandData>();
    data->topic            = argv[0];
    data->data             = NULL;
    data->task             = MQTTCommandTask::subscribe;
    DeviceLayer::PlatformMgr().ScheduleWork(MQTTCommandWorkerFunction, reinterpret_cast<intptr_t>(data));
    return CHIP_NO_ERROR;
}

CHIP_ERROR MQTTUnsubHandler(int argc, char ** argv)
{
    if (argc != 1)
    {
        return MQTTSubCommandHelpHandler(argc, argv);
    }
    MQTTCommandData * data = Platform::New<MQTTCommandData>();
    data->topic            = argv[0];
    data->data             = NULL;
    data->task             = MQTTCommandTask::unsubscribe;
    DeviceLayer::PlatformMgr().ScheduleWork(MQTTCommandWorkerFunction, reinterpret_cast<intptr_t>(data));
    return CHIP_NO_ERROR;
}

CHIP_ERROR MQTTPubHandler(int argc, char ** argv)
{
    if (argc != 2)
    {
        return MQTTSubCommandHelpHandler(argc, argv);
    }
    MQTTCommandData * data = Platform::New<MQTTCommandData>();
    data->topic            = argv[0];
    data->data             = argv[1];
    data->task             = MQTTCommandTask::publish;
    DeviceLayer::PlatformMgr().ScheduleWork(MQTTCommandWorkerFunction, reinterpret_cast<intptr_t>(data));
    return CHIP_NO_ERROR;
}

#endif // ENABLE_CHIP_SHELL

} // namespace shell

namespace shell {
void RegisterMQTTCommands()
{
    using namespace shell;
    static const shell_command_t sMQTTSubCommands[] = { { &MQTTSubCommandHelpHandler, "help", "Usage: mqtt <subcommand>" },
                                                        { &MQTTSubHandler, "sub", " Usage: cluster sub <topic>" },
                                                        { &MQTTUnsubHandler, "unsub", " Usage: cluster unsub <topic>" },
                                                        { &MQTTPubHandler, "pub", " Usage: cluster pub <topic> <data>" } };

    static const shell_command_t sMQTTCommand = { &MQTTCommandHandler, "mqtt", "MQTT commands. Usage: mqtt <subcommand>" };

    sShellMQTTSubCommands.RegisterCommands(sMQTTSubCommands, ArraySize(sMQTTSubCommands));

    Engine::Root().RegisterCommands(&sMQTTCommand, 1);
}

void MQTTCommandWorkerFunction(intptr_t context)
{
    if (context == 0)
    {
        ChipLogError(NotSpecified, "MQTTCommandWorkerFunction - Invalid work data");
        return;
    }
    shell::MQTTCommandData * data = reinterpret_cast<shell::MQTTCommandData *>(context);
    ESP_LOGI(TAG, " - Topic: '%s'", data->topic);
    if (data->data)
    {
        ESP_LOGI(TAG, " - Data: '%s'", data->data);
    }
    MQTTManager::GetInstance().ProcessCommand(data);
}
} // End namespace shell