package com.matterhub.server.entities.payloads;

import org.json.JSONArray;
import org.json.JSONObject;

import com.fasterxml.jackson.databind.JsonNode;
import com.matterhub.server.entities.MessageType;
import com.matterhub.server.entities.Metric;

public final class BasePayload extends Payload {
    private JSONObject payload;
    private MessageType messageType;
    private Metric[] metrics;

    public BasePayload(MessageType messageType, JsonNode payload) {
        super(payload);
        this.payload = payload;
        this.messageType = messageType;
        JSONArray metrics = this.payload.getJSONArray("metrics");
        this.metrics = new Metric[metrics.length()];
        for (int i = 0; i < metrics.length(); i++) {
            this.metrics[i] = new Metric(metrics.getJSONObject(i), messageType);
        }
    }

    public BasePayload(MessageType messageType, long timestamp, Object value, Metric... metrics) {
        super(0, timestamp);
        this.payload = new JSONObject();
        this.messageType = messageType;
        setTimestamp(timestamp);
        setMetrics(metrics);
        setSeq(0);
    }

    public long getTimestamp() {
        return this.payload.getLong("timestamp");
    }

    public void setTimestamp(long timestamp) {
        this.payload.put("timestamp", timestamp);
    }

    public Metric[] getMetrics() {
        return this.metrics;
    }

    public void setMetrics(Metric... metrics) {
        JSONArray jsonArray = new JSONArray();
        for (int i = 0; i < metrics.length; i++) {
            jsonArray.put(metrics[i].getMetric());
        }
        this.payload.put("metrics", jsonArray);
    }

    public void setSeq(int seq) {
        this.payload.put("seq", seq);
    }

    public void setValue(Object value) {
        this.payload.put("value", value);
    }

    public MessageType getMessageType() {
        return messageType;
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
}
