package com.matterhub.server.entities.dto;

import org.json.JSONObject;

import com.matterhub.server.entities.matter.Cluster;

public sealed class Metric permits CommandMetric, AttributeMetric{

    private JSONObject metric;
    private Cluster cluster;
    private String attributeId = "";
    private String command = "";
    private String argument = "";
    private String type = "";
    private MessageType messageType;

    public Metric(JSONObject metric, MessageType messageType) {
        assert metric != null;
        this.metric = metric;
        assert messageType != null;
        this.messageType = messageType;
        String[] values = metric.getString("name").split("/");
        if (values.length < 3) {
            return;
        }
        this.endpointId = values[0];
        this.clusterId = values[1];
        this.attributeId = values[2];
        if (messageType.equals(MessageType.DDATA)) {
            
        }
        if (messageType.equals(MessageType.DCMD)) {
            if (values.length < 4) {
                return;
            }
            this.endpointId = values[0];
            this.clusterId = values[1];
            this.command = values[2];
            this.argument = values[3];
        }
        if (messageType.equals(MessageType.DBIRTH)) {
            if (values.length < 4) {
                return;
            }
            this.endpointId = values[0];
            this.clusterId = values[1];
            this.type = values[2];
            if (this.type.equals("attribute")) {
                this.attributeId = values[3];
            } else if (this.type.equals("command")) {
                this.commandId = values[3];
            }

        }
    }

    public Metric(String name, long timestamp, Object value, MessageType messageType) {
        this.metric = new JSONObject();
        this.messageType = messageType;
        setName(name);
        setTimestamp(timestamp);
        setValue(value);
    }

    public String getEndpointId() {
        return this.endpointId;
    }

    public String getClusterId() {
        return this.clusterId;
    }

    public String getAttributeId() {
        return this.attributeId;
    }

    public String getCommand() {
        return this.command;
    }

    public String getArgument() {
        return this.argument;
    }

    public String getName() {
        return this.metric.getString("name");
    }

    public JSONObject getMetric() {
        return this.metric;
    }

    public MessageType getMessageType() {
        return this.messageType;
    }

    public void setName(String name) {
        this.metric.put("name", name);
    }

    public void setName(String endpointId, String clusterId, String attributeId) {
        this.metric.put("name", endpointId + "/" + clusterId + "/" + attributeId);
    }

    public long getTimestamp() {
        return this.metric.getLong("timestamp");
    }

    public void setTimestamp(long timestamp) {
        this.metric.put("timestamp", timestamp);
    }

    public String getDataType() {
        return String.valueOf(this.metric.get("dataType"));
    }

    public void setDataType(Object value) {
        if (value instanceof String) {
            this.metric.put("dataType", "String");
        } else if (value instanceof Boolean) {
            this.metric.put("dataType", "Boolean");
        } else if (value instanceof Integer) {
            this.metric.put("dataType", "Integer");
        } else {
            this.metric.put("dataType", value.getClass());
        }
    }

    public String toString() {
        return this.metric.toString();
    }

    // ---------------------------------------//
    // TODO all methods below are not generic!//
    // ---------------------------------------//

    public Object getValue() {
        if (!this.metric.has("value")) {
            return "0";
        }
        Object obj = this.metric.get("value");
        if (obj instanceof Boolean o) {
            if (o) {
                return "1";
            } else {
                return "0";
            }
        }
        return this.metric.get("value");
    }

    public void setValue(Object value) {
        // TODO What if 0 or 1 is just a number?
        if (value.toString().equals("0")) {
            this.metric.put("value", false);
            this.metric.put("dataType", "Boolean");
        } else if (value.toString().equals("1")) {
            this.metric.put("dataType", "Boolean");
            this.metric.put("value", true);
        } else {
            this.metric.put("value", value);
        }
    }

    public String getAttribute() {
        if (attributeId.equals("0")) {
            return "on";
        }
        return "";
    }

    public void setAttribute(String attribute) {
        if (attribute.equals("on")) {
            this.attributeId = "0";
        }
    }

}
