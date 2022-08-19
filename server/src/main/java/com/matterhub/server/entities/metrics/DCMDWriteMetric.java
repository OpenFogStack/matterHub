package com.matterhub.server.entities.metrics;

import com.fasterxml.jackson.databind.JsonNode;

public class DCMDWriteMetric
        extends DCMDMetric {
    private final int attributeId;

    public DCMDWriteMetric(short endpointId, int clusterId, int attributeId, long timestamp, DataType dataType,
                           JsonNode value) {
        super(endpointId, clusterId, timestamp, dataType, value);
        this.attributeId = attributeId;
    }


    @Override
    protected String lastPartOfName() {
        return "write/" + attributeId;
    }
}
