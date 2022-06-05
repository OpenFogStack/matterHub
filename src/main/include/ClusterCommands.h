#pragma once

void RegisterClusterCommands();

struct ClusterCommandData
{
    chip::FabricId fabricId;
    chip::NodeId nodeId;
    chip::EndpointId endpointId;
    chip::CommandId commandId;
    chip::ClusterId clusterId;
};

void ClusterCommandWorkerFunction(intptr_t context);
