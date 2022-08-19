package com.matterhub.server.unit;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.matterhub.server.WorldState;
import com.matterhub.server.entities.MessageType;
import com.matterhub.server.entities.Topic;
import com.matterhub.server.entities.matter.Cluster;
import com.matterhub.server.entities.matter.Endpoint;
import com.matterhub.server.entities.matter.Node;
import com.matterhub.server.entities.matter.generated.onoff.OnOffCluster;
import com.matterhub.server.entities.metrics.DataType;
import com.matterhub.server.entities.metrics.MetricDTO;
import com.matterhub.server.entities.payloads.DBirthPayload;
import com.matterhub.server.entities.payloads.Payload;
import com.matterhub.server.entities.payloads.PayloadDTO;
import org.json.JSONException;
import org.junit.jupiter.api.Test;

import java.io.IOException;
import java.util.HashSet;
import java.util.List;
import java.util.Optional;
import java.util.Set;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertInstanceOf;


public class MessageParsingTest {

    @Test
    public void TestDbirthMessage() throws JSONException, IOException {
        WorldState state = WorldState.worldState();
        var om = new ObjectMapper();
        JsonNode value;
        try (var is = this.getClass().getClassLoader().getResourceAsStream("dbirth.json")) {
            assert is != null;
            value = om.readTree(new String(is.readAllBytes()));
        }
        Topic topic = new Topic(value.get("topic").asText());
        assertEquals(MessageType.DBIRTH, topic.getMessageType());
        assertEquals(Optional.of(333L), topic.getMatterNodeId());
        Node topicNode = topic.toNode().orElseThrow();
        JsonNode message = value.get("message");
        Payload payload = Payload.parseMessage(topic.getMessageType(), message);
        assertInstanceOf(DBirthPayload.class, payload);
        DBirthPayload dBirthPayload = (DBirthPayload) payload;
        List<Endpoint> endpointList = dBirthPayload.getEndpoints();
        assertEquals(1, endpointList.size());
        Endpoint e = endpointList.get(0);
        Set<Cluster> cl = e.Clusters();
        assertEquals(1, cl.size());
        Cluster c = cl.stream().findFirst().orElseThrow();
        assertInstanceOf(OnOffCluster.class, c);
        OnOffCluster onOffCluster = (OnOffCluster) c;
        Node newNode = new Node(topicNode.hub(), topicNode.Id(), new HashSet<>(endpointList));
        state.apply(newNode.hub());
    }
    @Test
    public void TestPayloadDTOSerialization() throws JsonProcessingException {
        PayloadDTO dto = new PayloadDTO(MessageType.DCMD,System.currentTimeMillis() / 1000L, new MetricDTO("1/6/write/cmd/1", null, DataType.None.name(),System.currentTimeMillis() / 1000L));
        dto.toPayload();
    }

    @Test
    public void TestBasicDTOParsing() throws JSONException, IOException {
        var om = new ObjectMapper();
        JsonNode value;
        try (var is = this.getClass().getClassLoader().getResourceAsStream("cmddto.json")) {
            assert is != null;
            value = om.readTree(new String(is.readAllBytes()));
        }
        Topic topic = new Topic(value.get("topic").asText());
        assertEquals(MessageType.DCMD, topic.getMessageType());
        assertEquals(Optional.of(333L), topic.getMatterNodeId());
        Node topicNode = topic.toNode().orElseThrow();
        JsonNode message = value.get("message");

        Payload payload = Payload.parseMessage(topic.getMessageType(), message);

    }
}
