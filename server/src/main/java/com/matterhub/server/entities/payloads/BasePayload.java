package com.matterhub.server.entities.payloads;

import org.json.JSONArray;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.matterhub.server.entities.MessageType;
import com.matterhub.server.entities.Metric;

public final class BasePayload extends Payload {
    private MessageType messageType;
    private Metric[] metrics;

    public BasePayload(MessageType messageType, long timestamp, Metric... metrics) {
        super(0, timestamp);
        this.messageType = messageType;
        setTimestamp(timestamp);
        setMetrics(metrics);
        setSeq(0);
    }

    public Metric[] getMetrics() {
        return this.metrics;
    }

    public void setMetrics(Metric... metrics) {
        JSONArray jsonArray = new JSONArray();
        for (int i = 0; i < metrics.length; i++) {
            jsonArray.put(metrics[i].getMetric());
        }
    }

    public MessageType getMessageType() {
        return messageType;
    }
    
    protected void setMessageType(MessageType messageType) {
        this.messageType = messageType;
    }
    @Override
    public boolean equals(Object other) {
        if (other == null) {
            return false;
        }
        if (other instanceof BasePayload) {
            return false;
        }
        BasePayload otherBP = (BasePayload) other;
        if (messageType.equals(otherBP.getMessageType())) {
            Metric[] ownMetrics = this.getMetrics();
            Metric[] otherMetrics = otherBP.getMetrics();
            if (ownMetrics.length != otherMetrics.length) {
                return false;
            }
            int counter = 0;
            for (int i = 0; i < ownMetrics.length; i++) {
                for (int j = 0; j < otherMetrics.length; j++) {
                    if (ownMetrics[i].equals(otherMetrics[j])) {
                        counter++;
                    }
                }
            }
            if (counter == ownMetrics.length) {
                return true;
            }
        }
        return false;
    }

    public byte[] getPayload() throws JsonProcessingException {
        return OBJECT_MAPPER.writeValueAsString(this).getBytes();
    }
}
