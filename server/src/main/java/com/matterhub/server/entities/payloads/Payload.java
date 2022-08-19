package com.matterhub.server.entities.payloads;

import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.matterhub.server.entities.MessageType;
import lombok.Builder;

public sealed abstract class Payload permits PayloadDTO, DBirthPayload, DDataPayload{

    @JsonIgnore
    protected static final ObjectMapper OBJECT_MAPPER= new ObjectMapper();
    public static Payload parseMessage(MessageType messageType, JsonNode payload) throws JsonProcessingException, IllegalArgumentException {
        switch (messageType) {
            case DBIRTH:
                return OBJECT_MAPPER.treeToValue(payload, DBirthPayload.class);
            default:
                PayloadDTO bp = OBJECT_MAPPER.treeToValue(payload, PayloadDTO.class);
                bp.setMessageType(messageType);
                return bp;
        }
    }

    private int seq = 0;
    private long timestamp;

    public Payload(int sequenceNumber, long timestamp) {
        this.seq = sequenceNumber;
        this.timestamp = timestamp;
    }

    public abstract MessageType getMessageType();

    public long getTimestamp() {
        return this.timestamp;
    }
    public void setTimestamp(long timestamp) {
        this.timestamp = timestamp;
    }

    public int getSeq() {
        return this.seq;
    }
    public void setSeq(int seq) {
        this.seq = seq;
    }
}
