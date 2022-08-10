package com.matterhub.server.entities.dto;

import java.util.List;
import java.util.Map;

import com.matterhub.server.entities.matter.Cluster;
import com.matterhub.server.entities.matter.Endpoint;
import com.matterhub.server.entities.matter.generated.ClusterMapping;

record ClusterDTO(List<Integer> attributeIds, List<Integer> commandIds) {
};

record EndpointDTO(Map<Integer, ClusterDTO> clusters) {
    public List<Cluster> transformIntoClusters(){
        return clusters.entrySet().stream().map(entry -> 
            ClusterMapping.getClusterFromId(entry.getKey(), entry.getValue().attributeIds(), entry.getValue().commandIds())
        ).toList();
    }
};

record NodeDTO(Map<Short, EndpointDTO> endpoints) {
    public List<Endpoint> transformIntoEndpoints(){
        return endpoints.entrySet().stream().map(entry -> 
           {
            short endpointID = entry.getKey();
            List<Cluster> clusters = entry.getValue().transformIntoClusters();
            Endpoint endpoint =  new Endpoint(endpointID,clusters);
            return endpoint;
           }
        ).toList();
    }
};

public record CreationMessage(Map<Long, NodeDTO> nodes) {
};
