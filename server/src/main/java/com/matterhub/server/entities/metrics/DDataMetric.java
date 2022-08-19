package com.matterhub.server.entities.metrics;

import com.fasterxml.jackson.databind.JsonNode;
import com.matterhub.server.entities.matter.Cluster;
import com.matterhub.server.entities.matter.Endpoint;
import com.matterhub.server.entities.matter.generated.ClusterMapping;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;

public record DDataMetric(short endpointId, int clusterId, int attributeId, long timestamp, DataType dataType, JsonNode value)
        implements Metric {
    public Endpoint toEndpoint() {
        Cluster cl = ClusterMapping.getClusterFromId(this.clusterId, List.of(this.attributeId), List.of());
        cl.getAttribute(this.attributeId).fromMatterValue(this.value);
        var clusterList = new HashSet<Cluster>();
        clusterList.add(cl);
        return new Endpoint(endpointId, clusterList);
    }

    @Override
    public MetricDTO toDTO() {
        return null;
    }

    public static DDataMetric fromDTO(MetricDTO metricDTO) {
        DataType dt = DataType.valueOf(metricDTO.dataType());
        String [] strings = metricDTO.name().split("/");
        short endpointId = Short.parseShort(strings[0]);
        int clusterId= Integer.parseInt(strings[1]);
        int attributeId = Integer.parseInt(strings[2]);
        return  new DDataMetric(endpointId, clusterId, attributeId, metricDTO.timestamp(), dt, metricDTO.value());
    }
}

