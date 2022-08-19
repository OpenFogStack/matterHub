package com.matterhub.server.entities.metrics;

import com.fasterxml.jackson.databind.JsonNode;
import lombok.AllArgsConstructor;


@AllArgsConstructor
public abstract class DCMDMetric
        implements Metric {
    private final short endpointId;
    private final int clusterId;
    private final long timestamp;
    private final DataType dataType;
    private final JsonNode value;

    protected abstract String lastPartOfName();

    @Override
    public MetricDTO toDTO() {
        return new MetricDTO(endpointId + "/" + clusterId + "/" +
                             this.lastPartOfName(), this.value, this.dataType.name(), this.timestamp);
    }
}


class DCMDSubscribeMetric
        extends DCMDMetric {
    private final int attributeId;

    public DCMDSubscribeMetric(short endpointId, int clusterId, int attributeId, long timestamp) {
        super(endpointId, clusterId, timestamp, DataType.None, null);
        this.attributeId = attributeId;
    }


    @Override
    protected String lastPartOfName() {
        return "subscribe/" + attributeId;
    }
}

class DCMDReadMetric
        extends DCMDMetric {
    private final int attributeId;

    public DCMDReadMetric(short endpointId, int clusterId, int attributeId, long timestamp) {
        super(endpointId, clusterId, timestamp, DataType.None, null);
        this.attributeId = attributeId;
    }

    @Override
    protected String lastPartOfName() {
        return "read/" + attributeId;
    }
}
