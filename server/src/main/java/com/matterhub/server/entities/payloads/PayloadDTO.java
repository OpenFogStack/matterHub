package com.matterhub.server.entities.payloads;

import com.fasterxml.jackson.annotation.JsonIgnore;
import com.matterhub.server.entities.metrics.MetricDTO;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.matterhub.server.entities.MessageType;
import lombok.Builder;
import lombok.ToString;
import lombok.extern.jackson.Jacksonized;


@ToString
public final class PayloadDTO
        extends Payload {
    @JsonIgnore
    private MessageType messageType;
    private MetricDTO[] metrics;
    @Builder
    @Jacksonized
    public PayloadDTO(int seq, long timestamp, MetricDTO... metrics) {
        super(seq, timestamp);
        setTimestamp(timestamp);
        setMetrics(metrics);
    }

    public PayloadDTO(MessageType messageType, long timestamp, MetricDTO... metrics) {
        super(0, timestamp);
        this.messageType = messageType;
        setTimestamp(timestamp);
        setMetrics(metrics);
    }

    public MetricDTO[] getMetrics() {
        return this.metrics;
    }

    public void setMetrics(MetricDTO... metrics) {
        this.metrics = metrics;
    }

    public MessageType getMessageType() {
        return messageType;
    }
    
    void setMessageType(MessageType messageType) {
        this.messageType = messageType;
    }

    @Override
    public boolean equals(Object other) {
        if (other == null) {
            return false;
        }
        if (!(other instanceof PayloadDTO otherBP)) {
            return false;
        }
        if (messageType.equals(otherBP.getMessageType())) {
            MetricDTO[] ownMetrics = this.getMetrics();
            MetricDTO[] otherMetrics = otherBP.getMetrics();
            if (ownMetrics.length != otherMetrics.length) {
                return false;
            }
            int counter = 0;
            for (MetricDTO ownMetric : ownMetrics) {
                for (MetricDTO otherMetric : otherMetrics) {
                    if (ownMetric.equals(otherMetric)) {
                        counter++;
                    }
                }
            }
            return counter == ownMetrics.length;
        }
        return false;
    }

    public byte[] toPayload() throws JsonProcessingException {
        return OBJECT_MAPPER.writeValueAsString(this).getBytes();
    }
}
