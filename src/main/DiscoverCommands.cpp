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
#include <lib/dnssd/minimal_mdns/Server.h>
#include <lib/dnssd/minimal_mdns/QueryBuilder.h>
#include <lib/dnssd/MinimalMdnsServer.h>
#include <lib/dnssd/minimal_mdns/core/FlatAllocatedQName.h>
#include <mdns.h>
// #include <system/SystemPacketBuffer.h>



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
uint sSeq = 0;

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

CHIP_ERROR DiscoverNodesHandler(int argc, char ** argv)
{
    if (argc != 0)
    {
        return DiscoverHelpHandler(argc, argv);
    }
    ESP_LOGI("Discover", "DiscoverNodesHandler called");

    DeviceLayer::PlatformMgr().ScheduleWork(DiscoverNodesWorkerFunction, reinterpret_cast<intptr_t>(nullptr));
    return CHIP_NO_ERROR;
}

void RegisterDiscoverCommands()
{

    static const shell_command_t sDiscoverSubCommands[] = {
        { &DiscoverHelpHandler, "help", "Usage: discover <subcommand>" },
        { &DiscoverDescribeHandler, "describe", "Usage: describe <fabric id> <node id>" },
        { &DiscoverNodesHandler, "nodes", "Usage: nodes"}
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

void DiscoverNodesWorkerFunction(intptr_t context)
{
    ESP_LOGI("Discover", "MDNS Discovery running");
    mdns_result_t * results = nullptr;
    esp_err_t err = mdns_query_ptr("_matter","_tcp",5000,20,&results);
    ESP_LOGI("Discover", "MDNS Query Done");

    if(err == ESP_OK){
        mdns_result_t * current = results;
        ESP_LOGI("Discover", "Printing MDNS results");
        ESP_LOGI("Discover", "Results Addr: %p", results);

        while(current != nullptr){
            ESP_LOGI("Discover", "service name: %s, host name: %s, instance name: %s",current->service_type, current->hostname, current->instance_name);
            current = current->next;
        }
        mdns_query_results_free(results);
    }else {
        ESP_LOGE("Discover", "MDNS Error: 0x%02x", err);
    }
    
    ESP_LOGI("Discover", "MDNS Discovering nodes done!");
}



//this kind of format is much more efficient and removes redundancy
void PublishToMqttCompact(intptr_t context){
    ESP_LOGI("Discover", "Starting JSON");
    DescriptionManager * manager = reinterpret_cast<shell::DescriptionManager *>(context);

    char * topic = (char *) chip::Platform::MemoryAlloc(sizeof(char) * 256);
    snprintf(topic, 256, "spBv1.0/matterhub/DBIRTH/%d/%llu", CONFIG_MATTERHUBID, manager->mDevice->GetDeviceId());

    cJSON * dbirth = cJSON_CreateObject();
    cJSON_AddNumberToObject(dbirth,"timestamp", esp_log_timestamp());
    cJSON * metrics = cJSON_AddArrayToObject(dbirth,"metrics");
    char name[64];
    for(auto& endpoint:manager->mEndpoints){
        cJSON * metric = cJSON_CreateObject();
        snprintf(name,64,"%u",endpoint.first);
        cJSON_AddStringToObject(metric,"name",name);
        cJSON_AddNumberToObject(metric,"timestamp", esp_log_timestamp());
        cJSON_AddStringToObject(metric,"dataType", "String");


        cJSON * clusters = cJSON_CreateArray(); 
        for(auto& cluster:endpoint.second.clusters){
            cJSON * jCluster = cJSON_CreateObject();
            cJSON_AddNumberToObject(jCluster,"id", cluster.first);
            cJSON * attributes = cJSON_CreateIntArray((int *)cluster.second.attributes.data(),cluster.second.attributes.size());
            cJSON_AddItemToObject(jCluster,"attributes",attributes);
            cJSON * commands = cJSON_CreateIntArray((int *)cluster.second.commands.data(),cluster.second.commands.size());
            cJSON_AddItemToObject(jCluster,"commands",commands);
            cJSON_AddItemToArray(clusters,jCluster);
        }

        char * subJson = cJSON_PrintUnformatted(clusters);
        cJSON_AddStringToObject(metric,"value",subJson);

        cJSON_free(subJson);
        cJSON_Delete(clusters);

        cJSON_AddItemToArray(metrics,metric);
    }
    //this could potentially overflow some day, but then it is your own fault for describing so many nodes in a prototype ¯\_(ツ)_/¯
    cJSON_AddNumberToObject(dbirth, "seq", sSeq++);

    MQTTCommandData* data = Platform::New<MQTTCommandData>();
    data->data = cJSON_PrintUnformatted(dbirth);
    cJSON_Delete(dbirth);
    data->topic = topic;
    data->task = MQTTCommandTask::publish; 

    ESP_LOGV("Discover","%s\n%s",topic,data->data);
    MQTTManager::GetInstance().Publish(data);

    Platform::Delete(manager);
}

void PublishToMqttExpandedForm(intptr_t context){
    ESP_LOGI("Discover", "Starting JSON");
    DescriptionManager * manager = reinterpret_cast<shell::DescriptionManager *>(context);
    char * topic = (char *) chip::Platform::MemoryAlloc(sizeof(char) * 256);
    snprintf(topic, 256, "spBv1.0/matterhub/DBIRTH/%d/%llu", CONFIG_MATTERHUBID, manager->mDevice->GetDeviceId());

    cJSON * dbirth = cJSON_CreateObject();
    cJSON_AddNumberToObject(dbirth,"timestamp", esp_log_timestamp());
    std::vector<char*> names = {};
    cJSON * metrics = cJSON_AddArrayToObject(dbirth,"metrics");
    for(auto& endpoint:manager->mEndpoints){
        for(auto& cluster:endpoint.second.clusters){
            for(auto& attribute: cluster.second.attributes){
                cJSON * metric = cJSON_CreateObject();
                size_t const size = 64;  
                char* name = (char *)Platform::MemoryAlloc(size);
                names.push_back(name);
                snprintf(name, size, "%u/%u/attribute/%u", endpoint.first, cluster.first, attribute);
                cJSON_AddStringToObject(metric,"name", name);
                cJSON_AddNumberToObject(metric,"timestamp",esp_log_timestamp());
                cJSON_AddItemToArray(metrics,metric);
            }
            for(auto& command: cluster.second.commands){
                cJSON * metric = cJSON_CreateObject();
                size_t const size = 64;  
                char* name = (char *)Platform::MemoryAlloc(size);
                names.push_back(name);
                snprintf(name, size, "%u/%u/command/%u", endpoint.first, cluster.first, command);
                cJSON_AddStringToObject(metric,"name", name);
                cJSON_AddNumberToObject(metric,"timestamp",esp_log_timestamp());
                cJSON_AddItemToArray(metrics,metric);
            }
        }
    }
    cJSON_AddNumberToObject(dbirth, "seq", sSeq++);
    Platform::Delete(manager);

    MQTTCommandData* data = Platform::New<MQTTCommandData>();
    //printing this unformatted saves almost 2kb of ram :O
    data->data = cJSON_PrintUnformatted(dbirth);
    cJSON_Delete(dbirth);
    data->topic = topic;
    data->task = MQTTCommandTask::publish; 

    ESP_LOGI("Discover","%s\n%s",topic,data->data);
    MQTTManager::GetInstance().Publish(data);

    for(auto name: names){
        Platform::MemoryFree(name);
    }
}

void onCommandsReadCallback(const chip::app::ConcreteDataAttributePath& path, chip::TLV::TLVReader* data, void* context){
    DescriptionManager * manager = reinterpret_cast<shell::DescriptionManager *>(context);
    ESP_LOGI("Discover", "Command list for node: '0x%02llx' endpoint: '0x%02x' cluster '0x%02x'", manager->mDevice->GetDeviceId(), path.mEndpointId, path.mClusterId);

    auto list = chip::app::Clusters::Basic::Attributes::AcceptedCommandList::TypeInfo::DecodableType();
    
    chip::TLV::TLVReader reader;
    data->OpenContainer(reader);
    list.SetReader(reader);

    size_t size = 0;
    if(list.ComputeSize(&size)==CHIP_NO_ERROR){
        ESP_LOGI("Discover", " - Commands: '%u'", size);
    }

    ESP_LOGV("Discover", " - Command IDs:");
    auto cluster = &manager->currentCluster->second;
    for(auto it = list.begin();it.Next();){
        auto commandId = it.GetValue();
        cluster->commands.push_back(commandId);
        ESP_LOGV("Discover", "   - Command: '0x%02x'", commandId);
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
            // DeviceLayer::PlatformMgr().ScheduleWork(publishToMqtt, reinterpret_cast<intptr_t>(manager));
            DeviceLayer::PlatformMgr().ScheduleWork(PublishToMqttCompact, reinterpret_cast<intptr_t>(manager));
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
    ESP_LOGV("Discover", " - Attribute IDs:");
    auto cluster = &manager->currentCluster->second;
    for(auto it = list.begin();it.Next();){
        auto attributeId = it.GetValue();
        cluster->attributes.push_back(attributeId);
        ESP_LOGV("Discover", "   - Attribute: '0x%02x'", attributeId);
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
    ESP_LOGV("Discover", " - Cluster IDs:");
    auto endpoint = &manager->currentEndpoint->second;
    for(auto it = list.begin();it.Next();){
        auto clusterId = it.GetValue();
        endpoint->clusters.emplace(clusterId,Cluster(clusterId));
        ESP_LOGV("Discover", "   - Cluster: '0x%02x'", clusterId);
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
    ESP_LOGV("Discover", " - Attribute TLV type: '0x%02x'", type);

    auto partsList = chip::app::Clusters::Descriptor::Attributes::PartsList::TypeInfo::DecodableType();

    chip::TLV::TLVReader reader;
    data->OpenContainer(reader);
    partsList.SetReader(reader);
    size_t size = 0;
    if (partsList.ComputeSize(&size) == CHIP_NO_ERROR)
    {
        ESP_LOGI("Discover", " - Endpoints: '%u'", size);
    }
    ESP_LOGV("Discover", " - Endpoint IDs:");

    for (auto it = partsList.begin(); it.Next();)
    {
        auto endpointId = it.GetValue();

        manager->mEndpoints.emplace(endpointId,Endpoint(endpointId));
        ESP_LOGV("Discover", "   - Endpoint: '0x%02x'", endpointId);
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