package com.matterhub.server.unit;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertInstanceOf;

import java.io.IOException;

import org.json.JSONException;
import org.json.JSONObject;
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
        var is =this.getClass().getResourceAsStream("dbirth.json");
        JsonNode value = om.readTree(new String(is.readAllBytes()));
        Topic topic = new Topic(value.get("topic").asText());
        assertEquals(MessageType.DBIRTH, topic.getMessageType());
        JsonNode message = value.get("message");
        Payload payload = Payload.parseMessage(topic.getMessageType(), message);
        assertInstanceOf(DBirthPayload.class, payload);
        DBirthPayload dBirthPayload = (DBirthPayload) payload;
    }
}
