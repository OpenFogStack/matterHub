package com.matterhub.server.entities.payloads;

import com.matterhub.server.entities.MessageType;
import com.matterhub.server.entities.matter.Cluster;
import com.matterhub.server.entities.metrics.DDataMetric;

import java.util.List;

public final class DDataPayload extends Payload {
    List<DDataMetric> metrics;
    public DDataPayload(int sequenceNumber, long timestamp, List<DDataMetric> metrics) {
        super(sequenceNumber, timestamp);
        this.metrics = metrics;
    }

    @Override
    public MessageType getMessageType() {
        return MessageType.DDATA;
    }

    public List<Cluster> getClusters() {
       return metrics.stream().map(DDataMetric::toCluster).toList();
    }
}
