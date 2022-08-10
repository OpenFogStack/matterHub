package com.matterhub.cache;

import java.util.LinkedList;
import java.util.OptionalLong;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.matterhub.server.entities.dto.MessageType;
import com.matterhub.server.entities.dto.Metric;
import com.matterhub.server.entities.dto.MqttMatterMessage;

public class Cache {

    private static final int CAPACITY = 20;
    private static final LinkedList<MqttMatterMessage> list = new LinkedList<>();
    private static final Logger logger = LoggerFactory.getLogger(Cache.class);

    public static boolean checkIfMessageIsInCache(int hubId, OptionalLong nodeId, String endpoint, String cluster,
            String attributeId, Object value) {

        for (MqttMatterMessage message : list) {
            int messageHubId = message.getTopic().getMatterHubId();
            OptionalLong messageNodeId = message.getTopic().getMatterNodeId();

            if (messageHubId == hubId && messageNodeId.equals(nodeId)
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
        logger.info("Message added to cache: " + message.toString());
        if (list.size() == CAPACITY) {
            list.removeFirst();
        }
        list.add(message);
    }

}