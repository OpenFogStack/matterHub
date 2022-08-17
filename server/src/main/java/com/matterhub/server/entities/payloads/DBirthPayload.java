package com.matterhub.server.entities.payloads;

import com.fasterxml.jackson.annotation.JsonIgnore;
import com.matterhub.server.entities.MessageType;
import com.matterhub.server.entities.matter.Cluster;
import com.matterhub.server.entities.matter.Endpoint;
import com.matterhub.server.entities.matter.generated.ClusterMapping;
import lombok.Builder;
import lombok.extern.jackson.Jacksonized;

import java.util.List;
import java.util.Objects;
import java.util.Optional;

record ClusterDTO(Integer id, List<Integer> attributes, List<Integer> commands) {
    public Cluster toCluster() {
        return ClusterMapping.getClusterFromId(this.id,
                this.attributes, this.commands);
    }
}

record EndpointDTO(short id, List<ClusterDTO> clusters) {
    public Endpoint toEndpoint() {
        return new Endpoint(this.id, this.clusters.stream().map(clusterDTO -> {try {
            return clusterDTO.toCluster();
        } catch (IllegalArgumentException e) {
            //FIXME: This is an absolutly terrible hotfix for not generating code
            return null;
        }}).filter(Objects::nonNull).toList());
    }
}


public final class DBirthPayload extends Payload {

    private final List<EndpointDTO> endpointDTOS;
    @JsonIgnore
    private Optional<List<Endpoint>> endpoints = Optional.empty();

    @Builder
    @Jacksonized
    public DBirthPayload(long timestamp, int seq, List<EndpointDTO> endpoints) {
        super(seq, timestamp);
        this.endpointDTOS = endpoints;
    }

    @Override
    public MessageType getMessageType() {
        return MessageType.DBIRTH;
    }

    public List<Endpoint> getEndpoints() {
        if (this.endpoints.isPresent()) {
            return this.endpoints.get();
        }
        List<Endpoint> endpoints = endpointDTOS.stream().map(EndpointDTO::toEndpoint).toList();
        this.endpoints = Optional.of(endpoints);
        return endpoints;
    }
}
