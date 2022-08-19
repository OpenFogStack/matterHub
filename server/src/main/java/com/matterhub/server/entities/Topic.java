package com.matterhub.server.entities;

import com.matterhub.server.entities.matter.Endpoint;
import com.matterhub.server.entities.matter.Matterhub;
import com.matterhub.server.entities.matter.Node;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.Objects;
import java.util.Optional;

/**
 * Topic captures the information contained in the MQTT topic
 * It can be used both to build a topic from the required
 * components and to parse and extract the information from a topic string.
 * <p>
 * There are two types of topics:
 * <ul>
 * <li>matterHub Topics - They represent a single matterHub. You most likely
 * only have one of those.
 * These don't have a Node or Endpoint ID</li>
 * <li>endpoint Topics - These represent a single endpoint, which we want to
 * represent as a thing on the Bosch side</li>
 * </ul>
 */
public class Topic {

    private static final String NAMESPACE = "spBv1.0";
    private static final String GROUP_ID = "matterhub";
    private final MessageType messageType;
    private final int matterHubId;
    private final Optional<Long> matterNodeId;
    private final Optional<Short> matterEndpointId;

    /**
     * The endpoint constructor
     */
    public Topic(MessageType messageType, int matterHubId, long matterNodeId, short matterEndpointId) {
        this.messageType = messageType;
        this.matterHubId = matterHubId;
        this.matterNodeId = Optional.of(matterNodeId);
        this.matterEndpointId = Optional.of(matterEndpointId);
    }

    /**
     * The matterHub constructor
     */
    public Topic(MessageType messageType, int matterHubId) {
        this.messageType = messageType;
        this.matterHubId = matterHubId;
        this.matterNodeId = Optional.empty();
        this.matterEndpointId = Optional.empty();
    }

    public Topic(String topic) {
        String[] elements = topic.split("/");
        assert NAMESPACE.equals(elements[0]);
        assert GROUP_ID.equals(elements[1]);
        this.messageType = MessageType.getMessageType(elements[2]);
        this.matterHubId = Integer.parseInt(elements[3]);

        if (elements.length == 4) {
            // Matterhub
            this.matterNodeId = Optional.empty();
            this.matterEndpointId = Optional.empty();
        } else if (elements.length == 5) {
            // Node
            this.matterNodeId = Optional.of(Long.parseLong(elements[4]));
            this.matterEndpointId = Optional.empty();
        } else if (elements.length == 6) {
            // Endpoint
            this.matterNodeId = Optional.of(Long.parseLong(elements[4]));
            this.matterEndpointId = Optional.of(Short.parseShort(elements[5]));
        } else {
            throw new IllegalArgumentException("Topic is malformed:" + topic);
        }

    }

    public Optional<Long> getMatterNodeId() {
        return this.matterNodeId.map(Long::valueOf);
    }

    public Integer getMatterHubId() {
        return this.matterHubId;
    }

    public String getThingId() {
        return this.toString();
    }

    public MessageType getMessageType() {
        return this.messageType;
    }

    public String toString() {
        String topic = NAMESPACE + "/" + GROUP_ID;
        topic += "/" + messageType + "/" + matterHubId;
        if (matterNodeId.isPresent()) {
            topic += "/" + matterNodeId.get();
        }
        if (matterEndpointId.isPresent()) {
            topic += "/" + matterEndpointId.get();
        }
        return topic;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        Topic topic = (Topic) o;
        return getThingId().equals(topic.getThingId());
    }

    public Matterhub toMatterHub() {
        return new Matterhub(this.matterHubId, new HashSet<>());
    }

    public Optional<Node> toNode() {
        return this.matterNodeId.map(nodeId -> new Node(toMatterHub(), nodeId,  new HashSet<>()));
    }

    public Optional<Endpoint> toEndpoint() {
        return this.toNode().flatMap(node -> this.matterEndpointId.map(endpointId ->
                new Endpoint(node, endpointId,  new HashSet<>())));
    }

    @Override
    public int hashCode() {
        return Objects.hash(messageType, matterHubId, matterNodeId, matterEndpointId);
    }
}
