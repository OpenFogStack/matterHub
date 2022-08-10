package com.matterhub.server.entities.dto;

import java.util.OptionalLong;

import org.eclipse.ditto.things.model.ThingId;


public class Topic {

    private static final String NAMESPACE = "spBv1.0";
    private static final String GROUP_ID = "matterhub";
    private final MessageType messageType;
    private final int matterHubId;
    private final OptionalLong matterNodeId = OptionalLong.empty();
    private final ThingId thingId;

    public Topic(MessageType messageType, int matterHubId, long matterNodeId) {
        this.messageType = messageType;
        this.matterHubId = matterHubId;
        this.matterNodeId = OptionalLong.of(matterNodeId);
        this.thingId = ThingId.of(namespace, matterHubId + "_" + matterNodeId);
    }

    public Topic(String topic) {
        String[] elements = topic.split("/");
        assert NAMESPACE.equals(elements[0]);
        assert GROUP_ID.equals(elements[1]);

        this.messageType = MessageType.getMessageType(elements[2]);
        this.matterHubId = Integer.parseInt(elements[3]);
        if (elements.length == 5) {
            this.matterNodeId = OptionalLong.of(Long.parseLong(elements[4]));
        }
        this.thingId = matterHubId + "_" + matterNodeId;
    }

    public OptionalLong getMatterNodeId() {
        return this.matterNodeId;
    }

    public int getMatterHubId() {
        return this.matterHubId;
    }

    public String getThingId() {
        return this.thingId;
    }

    public MessageType getMessageType() {
        return this.messageType;
    }

    public String toString() {
        String topic  = "/" + messageType + "/" + matterHubId;
        if (matterNodeId != null) {
            topic += "/" + matterNodeId;
        }
        return topic;
    }

    public boolean equals(Topic topic) {
        if (thingId == null) {
            return false;
        }
        if (this.thingId.equals(topic.getMatterNodeId()) && this.matterHubId.equals(topic.getMatterHubId())) {
            return true;
        }
        return false;
    }
}
