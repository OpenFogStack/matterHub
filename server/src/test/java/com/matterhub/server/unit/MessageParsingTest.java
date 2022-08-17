package com.matterhub.server.unit;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertInstanceOf;

import java.io.IOException;
import java.util.List;
import java.util.Optional;

import com.matterhub.server.entities.matter.Cluster;
import com.matterhub.server.entities.matter.Endpoint;
import com.matterhub.server.entities.matter.generated.onoff.OnOffCluster;
import org.json.JSONException;
import org.junit.jupiter.api.Test;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.matterhub.server.entities.MessageType;
import com.matterhub.server.entities.Topic;
import com.matterhub.server.entities.payloads.DBirthPayload;
import com.matterhub.server.entities.payloads.Payload;


public class MessageParsingTest {
    
    @Test
    public void TestDbirthMessage () throws JSONException, IOException {
        var om = new ObjectMapper();
        JsonNode value;
        try (var is = this.getClass().getClassLoader().getResourceAsStream("dbirth.json")) {
            assert is != null;
            value = om.readTree(new String(is.readAllBytes()));
        }
        Topic topic = new Topic(value.get("topic").asText());
        assertEquals(MessageType.DBIRTH, topic.getMessageType());
        assertEquals(Optional.of(333L),topic.getMatterNodeId());
        JsonNode message = value.get("message");
        Payload payload = Payload.parseMessage(topic.getMessageType(), message);
        assertInstanceOf(DBirthPayload.class, payload);
        DBirthPayload dBirthPayload = (DBirthPayload) payload;
        List<Endpoint> endpointList = dBirthPayload.getEndpoints();
        assertEquals(1, endpointList.size());
        Endpoint e = endpointList.get(0);
        List<Cluster> cl = e.Clusters();
        assertEquals(1, cl.size());
        Cluster c = cl.get(0);
        assertInstanceOf(OnOffCluster.class, c);
        OnOffCluster onOffCluster = (OnOffCluster) c;


    }
}
