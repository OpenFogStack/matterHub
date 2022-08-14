package com.matterhub.server.entities.payloads;

import org.json.JSONObject;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.JsonNode;
import com.matterhub.server.entities.MessageType;

import java.util.List;
import java.util.Map;

import com.matterhub.server.entities.matter.Cluster;
import com.matterhub.server.entities.matter.Endpoint;
import com.matterhub.server.entities.matter.generated.ClusterMapping;

record ClusterDTO(List<Integer> attributeIds, List<Integer> commandIds) {
};

record EndpointDTO(Short id, Map<Integer, ClusterDTO> clusters) {
    public List<Cluster> transformIntoClusters() {
        return clusters.entrySet().stream().map(entry -> ClusterMapping.getClusterFromId(entry.getKey(),
                entry.getValue().attributeIds(), entry.getValue().commandIds())).toList();
    }
};

record NodeDTO(Map<Short, EndpointDTO> endpoints) {
    public List<Endpoint> transformIntoEndpoints() {
        return endpoints.entrySet().stream().map(entry -> {
            short endpointID = entry.getKey();
            List<Cluster> clusters = entry.getValue().transformIntoClusters();
            Endpoint endpoint = new Endpoint(endpointID, clusters);
            return endpoint;
        }).toList();
    }
};

public final class DBirthPayload extends Payload {

    private final Endpoint endpoint;

    public DBirthPayload(JsonNode payload) throws JsonProcessingException, IllegalArgumentException {
        super(payload);
        EndpointDTO dto = OBJECT_MAPPER.treeToValue(payload, EndpointDTO.class);
        List<Cluster> clusters = dto.transformIntoClusters();
        this.endpoint = new Endpoint(dto.id(), clusters);
    }

    @Override
    public MessageType getMessageType() {
        return MessageType.DBIRTH;
    }

    public Endpoint getEndpoint() {
        return this.endpoint;
    }
}
