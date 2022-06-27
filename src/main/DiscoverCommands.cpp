

#include "DiscoverCommands.h"
#include "ConnectionHelper.h"
#include "lib/shell/Engine.h"
#include "InteractionModel.h"

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

namespace shell {
/********************************************************
 * Discover shell functions
 *********************************************************/

Engine sDiscoverShellSubCommands;
 



CHIP_ERROR DiscoverHelpHandler(int argc, char ** argv)
{
    sDiscoverShellSubCommands.ForEachCommand(Shell::PrintCommandHelp, nullptr);
    return CHIP_NO_ERROR;
}

CHIP_ERROR DiscoverHandler(int argc, char ** argv)
{
    if (argc == 0)
    {
        return DiscoverHelpHandler(argc, argv);
    }

    return sDiscoverShellSubCommands.ExecCommand(argc, argv);
}

CHIP_ERROR DiscoverDescribeHandler(int argc, char ** argv){
     if (argc != 2)
    {
        return DiscoverHelpHandler(argc, argv);
    }
    DiscoverCommandData* entry = Platform::New<DiscoverCommandData>();
    entry->fabricId = atoi(argv[0]);
    entry->nodeId = atoi(argv[1]);
    ESP_LOGI("Discover", "DiscoverDescribeHandler called with:");
    // ESP_LOGI("Discover", " - Fabric ID: '0x%02x'", entry->fabricId );
    // ESP_LOGI("Discover", " - Node ID: '0x%02x'", entry->nodeId );


    DeviceLayer::PlatformMgr().ScheduleWork(DescribeWorkerFunction, reinterpret_cast<intptr_t>(entry));
    return CHIP_NO_ERROR;
}

void DescribeWorkerFunction(intptr_t context)
{
    VerifyOrReturn(context != 0, ChipLogError(NotSpecified, "DiscoverWorkerFunction - Invalid work data"));

    DiscoverCommandData * data = reinterpret_cast<DiscoverCommandData *>(context);

    ConnectionHelper::GetInstance().RequestConnection(data);

    Platform::Delete(data);
}
        
void RegisterDiscoverCommands()
{

    static const shell_command_t sDiscoverSubCommands[] = {
        { &DiscoverHelpHandler, "help", "Usage: discover <subcommand>" },
        { &DiscoverDescribeHandler, "describe",
          "Usage: describe <fabric id> <node id>" },
    };

    sDiscoverShellSubCommands.RegisterCommands(sDiscoverSubCommands,ArraySize(sDiscoverSubCommands));

    static const shell_command_t sDiscoverCommand = { &DiscoverHandler, "discover", " Usage: discover <subcommand>" };

    Engine::Root().RegisterCommands(&sDiscoverCommand, 1);
}

void onConnectionRequestCompleted(void * context, chip::OperationalDeviceProxy * peer_device)
{
    DiscoverCommandData * data = reinterpret_cast<shell::DiscoverCommandData *>(context);
    ESP_LOGI("Discover", "onConnectionRequestCompleted");

    
}


DiscoverCommandData::DiscoverCommandData() :
    BaseCommandData(onConnectionRequestCompleted, (void *) this)
{}
}
