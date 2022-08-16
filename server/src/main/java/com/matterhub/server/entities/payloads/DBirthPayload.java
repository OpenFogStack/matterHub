package com.matterhub.server.entities.payloads;

import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.databind.annotation.JsonDeserialize;
import com.matterhub.server.entities.MessageType;

import java.util.List;
import java.util.Map;
import java.util.Optional;

import com.matterhub.server.entities.matter.Cluster;
import com.matterhub.server.entities.matter.Endpoint;
import com.matterhub.server.entities.matter.generated.ClusterMapping;

import lombok.Builder;
import lombok.Value;

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

@JsonDeserialize(builder = DBirthPayload.DBirthPayloadBuilder.class)
public final class DBirthPayload extends Payload {

    @Builder
    public DBirthPayload(int sequenceNumber, long timestamp, EndpointDTO clusters) {
        super(sequenceNumber, timestamp);
        this.clusters = clusters;
    }

    private final EndpointDTO clusters;

    @JsonIgnore
    private Optional<Endpoint> endpoint = Optional.empty();

    @Override
    public MessageType getMessageType() {
        return MessageType.DBIRTH;
    }

    public Endpoint getEndpoint() {
        if (this.endpoint.isPresent()) {
            return this.endpoint.get();
        }
        List<Cluster> cl = clusters.transformIntoClusters();
        Endpoint endpoint = new Endpoint(clusters.id(), cl);
        this.endpoint = Optional.of(endpoint);
        return endpoint;
    }
}
