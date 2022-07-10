package com.matterhub.server.entities;

import org.json.JSONObject;

public class Metric {

    private JSONObject metric;
    private String endpointId = "";
    private String clusterId = "";
    private String attributeId = "";
    private String command = "";
    private String argument = "";
    private String type = "";
    private String commandId = "";
    private MessageType messageType;

    public Metric(JSONObject metric, MessageType messageType) {
        this.metric = metric;
        this.messageType = messageType;
        if (messageType == null) {
            return;
        }
        if (messageType.equals(MessageType.DDATA)) {
            String[] values = metric.getString("name").split("/");
            if (values.length < 3) {
                return;
            }
            this.endpointId = values[0];
            this.clusterId = values[1];
            this.attributeId = values[2];
        }
        if (messageType.equals(MessageType.DCMD)) {
            String[] values = metric.getString("name").split("/");
            if (values.length < 4) {
                return;
            }
            this.endpointId = values[0];
            this.clusterId = values[1];
            this.command = values[2];
            this.argument = values[3];
        }
        if (messageType.equals(MessageType.DBIRTH)) {
            String[] values = metric.getString("name").split("/");
            if (values.length < 4) {
                return;
            }
            this.endpointId = values[0];
            this.clusterId = values[1];
            this.type = values[2];
            if(this.type.equals("attribute")){
                this.attributeId = values[3];
            } else if (this.type.equals("command")){
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
        if(!this.metric.has("value")) {
            return "0";
        }
        Object o = this.metric.get("value");
        if (o instanceof Boolean) {
            if ((boolean) o) {
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

    public String getCluster() {
        if (clusterId.equals("6")) {
            return "on-off";
        }
        return "";
    }

    public void setCluster(String cluster) {
        if (cluster.equals("on-off")) {
            this.clusterId = "6";
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
