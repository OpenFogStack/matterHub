/*
 *    Published under MIT License
 *    Copyright (c) 2022 OpenFogStack
 *    Based on code written by Project CHIP Authors, which was published under Apache License, Version 2.0 (c) 2022
 */

#include "DiscoverCommands.h"
#include "ConnectionHelper.h"
#include "InteractionModel.h"
#include "Subscription.h"
#include "lib/shell/Engine.h"

#include "app/clusters/bindings/BindingManager.h"
#include "app/server/Server.h"
#include "controller/InvokeInteraction.h"
#include "platform/CHIPDeviceLayer.h"
#include <app/clusters/bindings/bindings.h>
#include <lib/support/CodeUtils.h>

#include "lib/shell/Engine.h"
#include "lib/shell/commands/Help.h"
#include "cJSON.h"
#include <sstream>


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

CHIP_ERROR DiscoverDescribeHandler(int argc, char ** argv)
{
    if (argc != 2)
    {
        return DiscoverHelpHandler(argc, argv);
    }
    DiscoverCommandData * entry = Platform::New<DiscoverCommandData>();
    entry->fabricId             = atoi(argv[0]);
    entry->nodeId               = atoi(argv[1]);
    ESP_LOGI("Discover", "DiscoverDescribeHandler called with:");
    ESP_LOGI("Discover", " - Fabric ID: '0x%02x'", atoi(argv[0]));
    ESP_LOGI("Discover", " - Node ID: '0x%02x'", atoi(argv[1]));

    DeviceLayer::PlatformMgr().ScheduleWork(DescribeWorkerFunction, reinterpret_cast<intptr_t>(entry));
    return CHIP_NO_ERROR;
}

void RegisterDiscoverCommands()
{

    static const shell_command_t sDiscoverSubCommands[] = {
        { &DiscoverHelpHandler, "help", "Usage: discover <subcommand>" },
        { &DiscoverDescribeHandler, "describe", "Usage: describe <fabric id> <node id>" },
    };

    sDiscoverShellSubCommands.RegisterCommands(sDiscoverSubCommands, ArraySize(sDiscoverSubCommands));

    static const shell_command_t sDiscoverCommand = { &DiscoverHandler, "discover", " Usage: discover <subcommand>" };

    Engine::Root().RegisterCommands(&sDiscoverCommand, 1);
}

void DescribeWorkerFunction(intptr_t context)
{
    VerifyOrReturn(context != 0, ChipLogError(NotSpecified, "DiscoverWorkerFunction - Invalid work data"));

    DiscoverCommandData * data = reinterpret_cast<DiscoverCommandData *>(context);

    ConnectionHelper::GetInstance().RequestConnection(data);
}

void PrintJson(intptr_t context){
    DescriptionManager * manager = reinterpret_cast<shell::DescriptionManager *>(context);

    cJSON *node = cJSON_CreateObject();
    cJSON_AddNumberToObject(node,"id",manager->mDevice->GetDeviceId());
    cJSON *endpoints = cJSON_AddArrayToObject(node,"endpoints");
    for(auto& endpoint:manager->mEndpoints){
        cJSON * jEndpoint = cJSON_CreateObject();
        cJSON_AddNumberToObject(jEndpoint,"id",endpoint.first);
        cJSON * clusters = cJSON_AddArrayToObject(jEndpoint,"clusters");
        for(auto& cluster:endpoint.second.clusters){
            cJSON * jCluster = cJSON_CreateObject();
            cJSON * attributes = cJSON_CreateIntArray((int *)cluster.second.attributes.data(),cluster.second.attributes.size());
            cJSON_AddItemToObject(jCluster,"attributes",attributes);
            cJSON * commands = cJSON_CreateIntArray((int *)cluster.second.commands.data(),cluster.second.commands.size());
            cJSON_AddItemToObject(jCluster,"commands",commands);
            cJSON_AddItemToArray(clusters,jCluster);
        }
        cJSON_AddItemToArray(endpoints, jEndpoint);
    }
    ESP_LOGI("Discover", "Description JSON:\n%s", cJSON_Print(node));

    Platform::Delete(manager);
}

void onCommandsReadCallback(const chip::app::ConcreteDataAttributePath& path, chip::TLV::TLVReader* data, void* context){
    DescriptionManager * manager = reinterpret_cast<shell::DescriptionManager *>(context);
    ESP_LOGI("Discover", "Attribute list for node: '0x%02llx' endpoint: '0x%02x' cluster '0x%02x'", manager->mDevice->GetDeviceId(), path.mEndpointId, path.mClusterId);

    auto list = chip::app::Clusters::Basic::Attributes::AcceptedCommandList::TypeInfo::DecodableType();
    
    chip::TLV::TLVReader reader;
    data->OpenContainer(reader);
    list.SetReader(reader);

    size_t size = 0;
    if(list.ComputeSize(&size)==CHIP_NO_ERROR){
        ESP_LOGI("Discover", " - Commands: '%u'", size);
    }

    ESP_LOGI("Discover", " - Command IDs:");
    auto cluster = &manager->currentCluster->second;
    for(auto it = list.begin();it.Next();){
        auto commandId = it.GetValue();
        cluster->commands.push_back(commandId);
        ESP_LOGI("Discover", "   - Command: '0x%02x'", commandId);
    }

    data->CloseContainer(reader);

    manager->currentCluster++;
    if(manager->currentCluster != manager->currentEndpoint->second.clusters.end()){
        manager->ReadAttribute(manager->currentEndpoint->first, manager->currentCluster->first, chip::app::Clusters::Basic::Attributes::AttributeList::Id,onAttributesReadCallback);
    }else{
        manager->currentEndpoint++;
        if(manager->currentEndpoint != manager->mEndpoints.end()){
            manager->ReadAttribute(manager->currentEndpoint->first, chip::app::Clusters::Descriptor::Id, chip::app::Clusters::Descriptor::Attributes::ServerList::Id,onServerListReadCallback);
        }else{
            //Schedule printing so we can release the callback asap
            //this is also a placeholder for a more meaningful callback
            DeviceLayer::PlatformMgr().ScheduleWork(PrintJson, reinterpret_cast<intptr_t>(manager));
        }
    }
}

void onAttributesReadCallback(const chip::app::ConcreteDataAttributePath& path, chip::TLV::TLVReader* data, void* context){
    DescriptionManager * manager = reinterpret_cast<shell::DescriptionManager *>(context);
    ESP_LOGI("Discover", "Attribute list for node: '0x%02llx' endpoint: '0x%02x' cluster '0x%02x'", manager->mDevice->GetDeviceId(), path.mEndpointId, path.mClusterId);

    auto list = chip::app::Clusters::Basic::Attributes::AttributeList::TypeInfo::DecodableType();
    chip::TLV::TLVReader reader;
    data->OpenContainer(reader);
    list.SetReader(reader);
    size_t size = 0;
    if(list.ComputeSize(&size)==CHIP_NO_ERROR){
        ESP_LOGI("Discover", " - Attributes: '%u'", size);
    }
    ESP_LOGI("Discover", " - Attribute IDs:");
    auto cluster = &manager->currentCluster->second;
    for(auto it = list.begin();it.Next();){
        auto attributeId = it.GetValue();
        cluster->attributes.push_back(attributeId);
        ESP_LOGI("Discover", "   - Attribute: '0x%02x'", attributeId);
    }
    data->CloseContainer(reader);
    manager->ReadAttribute(manager->currentEndpoint->first, manager->currentCluster->first, chip::app::Clusters::Basic::Attributes::AcceptedCommandList::Id,onCommandsReadCallback);
}

void onServerListReadCallback(const chip::app::ConcreteDataAttributePath& path, chip::TLV::TLVReader* data, void* context)
{
    DescriptionManager * manager = reinterpret_cast<shell::DescriptionManager *>(context);
    ESP_LOGI("Discover", "Server cluster list for node: '0x%02llx' endpoint: '0x%02x'", manager->mDevice->GetDeviceId(), path.mEndpointId);

    auto list = chip::app::Clusters::Descriptor::Attributes::ServerList::TypeInfo::DecodableType();
    chip::TLV::TLVReader reader;
    data->OpenContainer(reader);
    list.SetReader(reader);
    size_t size = 0;
    if(list.ComputeSize(&size)==CHIP_NO_ERROR){
        ESP_LOGI("Discover", " - Clusters: '%u'", size);
    }
    ESP_LOGI("Discover", " - Cluster IDs:");
    auto endpoint = &manager->currentEndpoint->second;
    for(auto it = list.begin();it.Next();){
        auto clusterId = it.GetValue();
        endpoint->clusters.emplace(clusterId,Cluster(clusterId));
        ESP_LOGI("Discover", "   - Cluster: '0x%02x'", clusterId);
    }
    data->CloseContainer(reader);

    manager->currentCluster = endpoint->clusters.begin();
    manager->ReadAttribute(endpoint->id, manager->currentCluster->first, chip::app::Clusters::Basic::Attributes::AttributeList::Id,onAttributesReadCallback);
}

void onPartListReadCallback(const chip::app::ConcreteDataAttributePath& path, chip::TLV::TLVReader* data, void* context){
    DescriptionManager * manager = reinterpret_cast<shell::DescriptionManager *>(context);
    
    chip::TLV::TLVType type = data->GetType();
    ESP_LOGI("Discover", "Partlist read successfully for:");
    ESP_LOGI("Discover", " - NodeId: '0x%02llx'", manager->mDevice->GetDeviceId());
    ESP_LOGI("Discover", " - Attribute TLV type: '0x%02x'", type);

    auto partsList = chip::app::Clusters::Descriptor::Attributes::PartsList::TypeInfo::DecodableType();

    chip::TLV::TLVReader reader;
    data->OpenContainer(reader);
    partsList.SetReader(reader);
    size_t size = 0;
    if (partsList.ComputeSize(&size) == CHIP_NO_ERROR)
    {
        ESP_LOGI("Discover", " - Endpoints: '%u'", size);
    }
    ESP_LOGI("Discover", " - Endpoint IDs:");

    for (auto it = partsList.begin(); it.Next();)
    {
        auto endpointId = it.GetValue();

        manager->mEndpoints.emplace(endpointId,Endpoint(endpointId));
        ESP_LOGI("Discover", "   - Endpoint: '0x%02x'", endpointId);
    }
    data->CloseContainer(reader);

    manager->currentEndpoint = manager->mEndpoints.begin();
    manager->ReadAttribute(manager->currentEndpoint->first, chip::app::Clusters::Descriptor::Id, chip::app::Clusters::Descriptor::Attributes::ServerList::Id,onServerListReadCallback);

}

void onConnectionRequestCompleted(void * context, chip::OperationalDeviceProxy * peer_device)
{
    DiscoverCommandData * data = reinterpret_cast<DiscoverCommandData*>(context); 
    Platform::New<DescriptionManager>(peer_device);
    Platform::Delete(data);
}

DiscoverCommandData::DiscoverCommandData() :
    BaseCommandData(onConnectionRequestCompleted, (void *) this)
{}
}