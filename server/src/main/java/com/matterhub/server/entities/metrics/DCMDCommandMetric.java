package com.matterhub.server.entities.metrics;

import com.fasterxml.jackson.databind.JsonNode;

public class DCMDCommandMetric
        extends DCMDMetric {

    private final int commandId;

    public DCMDCommandMetric(short endpointId, int clusterId, int commandId, long timestamp, DataType dataType,
                             JsonNode value) {
        super(endpointId, clusterId, timestamp, dataType, value);
        this.commandId = commandId;
    }
    @Override
    protected String lastPartOfName() {
        return "cmd/" + commandId;
    }
}
