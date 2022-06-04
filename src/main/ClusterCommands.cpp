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
#include "app/CommandSender.h"
#include "app/clusters/bindings/BindingManager.h"
#include "app/server/Server.h"
#include "controller/InvokeInteraction.h"
#include "platform/CHIPDeviceLayer.h"
#include <app/clusters/bindings/bindings.h>
#include <lib/support/CodeUtils.h>
#include "ClusterCommands.h"

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

Engine sShellClusterSubCommands;
Engine sShellClusterOnOffSubCommands;

#endif // defined(ENABLE_CHIP_SHELL)

static const char *TAG = "Cluster Commands";
namespace
{

/*
    void ProcessOnOffUnicastBindingCommand(CommandId commandId, const EmberBindingTableEntry &binding, DeviceProxy *peer_device)
    {
        auto onSuccess = [](const ConcreteCommandPath &commandPath, const StatusIB &status, const auto &dataResponse)
        {
            ChipLogProgress(NotSpecified, "OnOff command succeeds");
        };

        auto onFailure = [](CHIP_ERROR error)
        {
            ChipLogError(NotSpecified, "OnOff command failed: %" CHIP_ERROR_FORMAT, error.Format());
        };

        switch (commandId)
        {
        case Clusters::OnOff::Commands::Toggle::Id:
            Clusters::OnOff::Commands::Toggle::Type toggleCommand;
            Controller::InvokeCommandRequest(peer_device->GetExchangeManager(), peer_device->GetSecureSession().Value(), binding.remote,
                                             toggleCommand, onSuccess, onFailure);
            break;

        case Clusters::OnOff::Commands::On::Id:
            Clusters::OnOff::Commands::On::Type onCommand;
            Controller::InvokeCommandRequest(peer_device->GetExchangeManager(), peer_device->GetSecureSession().Value(), binding.remote,
                                             onCommand, onSuccess, onFailure);
            break;

        case Clusters::OnOff::Commands::Off::Id:
            Clusters::OnOff::Commands::Off::Type offCommand;
            Controller::InvokeCommandRequest(peer_device->GetExchangeManager(), peer_device->GetSecureSession().Value(), binding.remote,
                                             offCommand, onSuccess, onFailure);
            break;
        }
    }

    void LightSwitchChangedHandler(const EmberBindingTableEntry &binding, DeviceProxy *peer_device, void *context)
    {
        VerifyOrReturn(context != nullptr, ChipLogError(NotSpecified, "OnDeviceConnectedFn: context is null"));
        BindingCommandData *data = static_cast<BindingCommandData *>(context);

        if (binding.type == EMBER_MULTICAST_BINDING && data->isGroup)
        {
            switch (data->clusterId)
            {
            case Clusters::OnOff::Id:
                ProcessOnOffGroupBindingCommand(data->commandId, binding);
                break;
            }
        }
        else if (binding.type == EMBER_UNICAST_BINDING && !data->isGroup)
        {
            switch (data->clusterId)
            {
            case Clusters::OnOff::Id:
                ProcessOnOffUnicastBindingCommand(data->commandId, binding, peer_device);
                break;
            }
        }
    }

    void InitBindingHandlerInternal(intptr_t arg)
    {
        auto &server = chip::Server::GetInstance();
        chip::BindingManager::GetInstance().Init(
            {&server.GetFabricTable(), server.GetCASESessionManager(), &server.GetPersistentStorage()});
        chip::BindingManager::GetInstance().RegisterBoundDeviceChangedHandler(LightSwitchChangedHandler);
    }
*/
#ifdef CONFIG_ENABLE_CHIP_SHELL

    /********************************************************
     * Cluster shell functions
     *********************************************************/

    CHIP_ERROR ClusterHelpHandler(int argc, char **argv)
    {
        sShellClusterSubCommands.ForEachCommand(Shell::PrintCommandHelp, nullptr);
        return CHIP_NO_ERROR;
    }

    CHIP_ERROR ClusterCommandHandler(int argc, char **argv)
    {
        if (argc == 0)
        {
            return ClusterHelpHandler(argc, argv);
        }

        return sShellClusterSubCommands.ExecCommand(argc, argv);
    }

    /********************************************************
     * OnOff switch shell functions
     *********************************************************/

    CHIP_ERROR ClusterCommandOnOffHelpHandler(int argc, char **argv)
    {
        sShellClusterOnOffSubCommands.ForEachCommand(Shell::PrintCommandHelp, nullptr);
        return CHIP_NO_ERROR;
    }

    CHIP_ERROR ClusterCommandOnOffHandler(int argc, char **argv)
    {
        if (argc == 0)
        {
            return ClusterCommandOnOffHelpHandler(argc, argv);
        }

        return sShellClusterOnOffSubCommands.ExecCommand(argc, argv);
    }

    CHIP_ERROR ClusterCommandOnOffOnHandler(int argc, char **argv)
    {
        ESP_LOGI(TAG, "On Handler -> TODO");
        if (argc != 3)
        {
            return ClusterCommandOnOffHelpHandler(argc, argv);
        }
        ClusterCommandData *data = Platform::New<ClusterCommandData>();
        data->fabricId = atoi(argv[0]);
        data->nodeId = atoi(argv[1]);
        data->endpointId = atoi(argv[2]);
        data->commandId = Clusters::OnOff::Commands::On::Id;
        data->clusterId = Clusters::OnOff::Id;

        DeviceLayer::PlatformMgr()
            .ScheduleWork(ClusterCommandWorkerFunction, reinterpret_cast<intptr_t>(data));

        return CHIP_NO_ERROR;
    }

    CHIP_ERROR ClusterCommandOnOffOffHandler(int argc, char **argv)
    {
        ESP_LOGI(TAG, "Off Handler -> TODO");
        if (argc != 3)
        {
            return ClusterCommandOnOffHelpHandler(argc, argv);
        }
        ClusterCommandData *data = Platform::New<ClusterCommandData>();
        data->fabricId = atoi(argv[0]);
        data->nodeId = atoi(argv[1]);
        data->endpointId = atoi(argv[2]);
        data->commandId = Clusters::OnOff::Commands::Off::Id;
        data->clusterId = Clusters::OnOff::Id;

        DeviceLayer::PlatformMgr()
            .ScheduleWork(ClusterCommandWorkerFunction, reinterpret_cast<intptr_t>(data));
        return CHIP_NO_ERROR;
    }

    CHIP_ERROR ClusterCommandOnOffToggleHandler(int argc, char **argv)
    {
        ESP_LOGI(TAG, "Toggle Handler -> TODO");
        /*
        BindingCommandData *data = Platform::New<BindingCommandData>();
        data->commandId = Clusters::OnOff::Commands::Toggle::Id;
        data->clusterId = Clusters::OnOff::Id;

        DeviceLayer::PlatformMgr().ScheduleWork(ClusterCommandWorkerFunction, reinterpret_cast<intptr_t>(data));
        */
        return CHIP_NO_ERROR;
    }

    /**
     * @brief configures switch matter shell
     *
     */

#endif // ENABLE_CHIP_SHELL

} // namespace

/********************************************************
 * Switch functions
 *********************************************************/
namespace
{

    chip::PeerId PeerIdForNode(chip::FabricTable *fabricTable, chip::FabricIndex fabric, chip::NodeId node)
    {
        chip::FabricInfo *fabricInfo = fabricTable->FindFabricWithIndex(fabric);
        if (fabricInfo == nullptr)
        {
            return chip::PeerId();
        }
        return fabricInfo->GetPeerIdForNode(node);
    }
}

void onFailureCallbackClusterCommandOnOff(void *context, PeerId peerId, CHIP_ERROR error)
{
    auto &server = chip::Server::GetInstance();
    CASESessionManager *CASESessionManager = server.GetCASESessionManager();
    // Simply release the entry, the connection will be re-established as needed.
    ChipLogError(NotSpecified, "Failed to establish connection to node 0x" ChipLogFormatX64, ChipLogValueX64(peerId.GetNodeId()));
    CASESessionManager->ReleaseSession(peerId);
}

void onConnectedCallbackClusterCommandOnOff(void *context, OperationalDeviceProxy *peer_device)
{
    ChipLogError(NotSpecified, "Got here!!");
    ClusterCommandData *data = reinterpret_cast<ClusterCommandData *>(context);

    auto onSuccess = [data](const ConcreteCommandPath &commandPath, const StatusIB &status, const auto &dataResponse)
    {
        ChipLogProgress(NotSpecified, "OnOff command succeeds");
        Platform::Delete(data);
    };

    auto onFailure = [data](CHIP_ERROR error)
    {
        ChipLogError(NotSpecified, "OnOff command failed: %" CHIP_ERROR_FORMAT, error.Format());
        Platform::Delete(data);
    };
    auto commandId = data->commandId;
    switch (commandId)
    {
    case Clusters::OnOff::Commands::On::Id:
        Clusters::OnOff::Commands::On::Type onCommand;
        Controller::InvokeCommandRequest(peer_device->GetExchangeManager(), peer_device->GetSecureSession().Value(), data->endpointId,
                                         onCommand, onSuccess, onFailure);
        break;
    case Clusters::OnOff::Commands::Off::Id:
        Clusters::OnOff::Commands::Off::Type offCommand;
        Controller::InvokeCommandRequest(peer_device->GetExchangeManager(), peer_device->GetSecureSession().Value(), data->endpointId,
                                         offCommand, onSuccess, onFailure);
        break;
    case Clusters::OnOff::Commands::Toggle::Id:
        Clusters::OnOff::Commands::Toggle::Type toggleCommand;
        Controller::InvokeCommandRequest(peer_device->GetExchangeManager(), peer_device->GetSecureSession().Value(), data->endpointId,
                                         toggleCommand, onSuccess, onFailure);
        break;
    }
}

void ClusterCommandWorkerFunction(intptr_t context)
{
    if (context == 0)
    {
        ChipLogError(NotSpecified, "ClusterCommandWorkerFunction - Invalid work data");
        return;
    }
    ClusterCommandData *data = reinterpret_cast<ClusterCommandData *>(context);
    auto &server = chip::Server::GetInstance();
    chip::FabricTable *fabricTable = &server.GetFabricTable();
    CASESessionManager *CASESessionManager = server.GetCASESessionManager();
    if (CASESessionManager == nullptr)
    {
        ChipLogError(NotSpecified, "ClusterCommandWorkerFunction - Invalid SessionManager");
        return;
    }

    PeerId peer = PeerIdForNode(fabricTable, data->fabricId, data->nodeId);
    if (peer.GetNodeId() == kUndefinedNodeId)
    {
        ChipLogError(NotSpecified, "ClusterCommandWorkerFunction - Unable to find the mentioned Peer");
        return;
    }

    Callback::Callback<OnDeviceConnected> mOnConnectedCallback = Callback::Callback<OnDeviceConnected>(onConnectedCallbackClusterCommandOnOff, (void *)context);
    Callback::Callback<OnDeviceConnectionFailure> mOnConnectionFailureCallback = Callback::Callback<OnDeviceConnectionFailure>(onFailureCallbackClusterCommandOnOff, (void *)context);

    CASESessionManager->FindOrEstablishSession(peer, &mOnConnectedCallback, &mOnConnectionFailureCallback);
    ChipLogError(NotSpecified, "ClusterCommandWorkerFunction - Registration Done");
}

void RegisterClusterCommands()
{

    static const shell_command_t sClusterSubCommands[] = {
        {&ClusterHelpHandler, "help", "Usage: cluster <subcommand>"},
        {&ClusterCommandOnOffHandler, "onoff", " Usage: cluster onoff <subcommand>"},
    };

    static const shell_command_t sClusterCommandOnOffSubCommands[] = {
        {&ClusterCommandOnOffHelpHandler, "help", "Usage : cluster ononff <subcommand>"},
        {&ClusterCommandOnOffOnHandler, "on", "Usage: cluster onoff on <Fabric> <Node> <Endpoint>"},
        {&ClusterCommandOnOffOffHandler, "off", "Usage: cluster onoff off <Fabric> <Node> <Endpoint>"},
        {&ClusterCommandOnOffToggleHandler, "toggle", "Usage: cluster onoff off <Fabric> <Node> <Endpoint>"}};

    static const shell_command_t sClusterCommand = {&ClusterCommandHandler, "cluster",
                                                    "Cluster commands. Usage: cluster <subcommand>"};

    sShellClusterOnOffSubCommands.RegisterCommands(sClusterCommandOnOffSubCommands, ArraySize(sClusterCommandOnOffSubCommands));
    sShellClusterSubCommands.RegisterCommands(sClusterSubCommands, ArraySize(sClusterSubCommands));

    Engine::Root().RegisterCommands(&sClusterCommand, 1);
}