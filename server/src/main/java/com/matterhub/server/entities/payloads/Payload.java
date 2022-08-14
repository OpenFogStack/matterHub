package com.matterhub.server.entities.payloads;

import org.json.JSONObject;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.matterhub.server.entities.MessageType;

public sealed abstract class Payload permits BasePayload, DBirthPayload{

    protected static final ObjectMapper OBJECT_MAPPER= new ObjectMapper();
    public static Payload parseMessage(MessageType messageType, JsonNode payload) throws JsonProcessingException, IllegalArgumentException {
        switch (messageType) {
            case DBIRTH:
                return new DBirthPayload(payload);
            default:
                return new BasePayload(messageType, payload);
        }
    }
    private final int sequenceNumber;
    private final long timestamp;

    public Payload(JsonNode payload) {
        this.sequenceNumber = payload.get("seq").asInt(0);
        this.timestamp = payload.get("timestamp").asLong();
    }
    public Payload(int sequenceNumber, long timestamp) {
        this.sequenceNumber = sequenceNumber;
        this.timestamp = timestamp;
    }
    public long getTimestamp() {
        return this.timestamp;
    }

    public int getSeq() {
        return this.sequenceNumber;
    }

    public abstract MessageType getMessageType();

    public byte[] getPayload() throws JsonProcessingException {
        return OBJECT_MAPPER.writeValueAsString(this).getBytes();
    }
}
