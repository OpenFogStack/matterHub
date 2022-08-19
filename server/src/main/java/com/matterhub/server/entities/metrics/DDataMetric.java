package com.matterhub.server.entities.metrics;

import com.fasterxml.jackson.databind.JsonNode;
import com.matterhub.server.entities.matter.Cluster;
import com.matterhub.server.entities.matter.generated.ClusterMapping;

import java.util.List;

public record DDataMetric(int clusterId, int attributeId, long timestamp, DataType dataType, JsonNode value)
        implements Metric {
    public Cluster toCluster() {
        Cluster cl = ClusterMapping.getClusterFromId(this.clusterId, List.of(this.attributeId), List.of());
        cl.getAttribute(this.attributeId).fromMatterValue(this.value);
        return cl;
    }

    @Override
    public MetricDTO toDTO() {
        return null;
    }
}

