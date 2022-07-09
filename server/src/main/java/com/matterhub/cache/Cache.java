package com.matterhub.cache;

import java.util.LinkedList;

import com.matterhub.server.entities.MessageType;
import com.matterhub.server.entities.Metric;
import com.matterhub.server.entities.MqttMatterMessage;

public class Cache {

    private static final int CAPACITY = 20;
    private static LinkedList<MqttMatterMessage> list = new LinkedList<>();

    public static boolean checkIfMessageIsInCache(String hubId, String nodeId, String endpoint, String cluster,
            String attributeId, Object value) {

        for (int i = list.size() - 1; i >= 0; i--) {
            MqttMatterMessage message = list.get(i);

            String messageHubId = message.getTopic().getMatterHubId();
            String messageNodeId = message.getTopic().getMatterNodeId();

            if (messageHubId.equals(hubId) && messageNodeId.equals(nodeId)
                    && message.getPayload().getMetrics() != null) {

                Metric messageMetric = message.getPayload().getMetrics()[0];
                String messageEndpoint = messageMetric.getEndpointId();
                String messageCluster = messageMetric.getCluster();
                String messageAttributeID = "";
                String messageValue = "";
                if (message.getTopic().getMessageType() == MessageType.DBIRTH) {
                    if(value.equals("0")){
                        return true;
                    }
                }
                if (message.getTopic().getMessageType().equals(MessageType.DDATA)) {
                    messageAttributeID = messageMetric.getAttribute();
                    messageValue = messageMetric.getValue().toString();
                }
                if (message.getTopic().getMessageType().equals(MessageType.DCMD)) {
                    if (messageMetric.getCommand().equals("cmd")) {
                        String messageArgument = messageMetric.getArgument();
                        if (messageArgument.equals("1") || messageArgument.equals("on")) {
                            messageAttributeID = "on";
                            messageValue = "1";
                        }
                        if (messageArgument.equals("0") || messageArgument.equals("off")) {
                            messageAttributeID = "on";
                            messageValue = "0";
                        }
                    }
                }
                if (!messageEndpoint.isEmpty()) {
                    if ((messageEndpoint.equals(endpoint) && messageCluster.equals(cluster)
                            && messageAttributeID.equals(attributeId))) {
                        if (messageValue.equals(value)) {
                            return true;
                        } else {
                            return false;
                        }
                    }
                }
            }
        }

        return false;
    }

    public static void put(MqttMatterMessage message) {
        System.out.println("Message added to cache: " + message.toString());
        if (list.size() == CAPACITY) {
            list.removeFirst();
        }
        list.add(message);
    }

}