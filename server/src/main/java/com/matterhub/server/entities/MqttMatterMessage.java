package com.matterhub.server.entities;

public class MqttMatterMessage {

    private Topic topic;

    private Payload payload;
    
    public MqttMatterMessage(Topic topic, Payload payload) {
        this.topic = topic;
        this.payload = payload;
    }
    public Topic getTopic() {
        return this.topic;
    }

    public void setTopic(Topic topic) {
        this.topic = topic;
    }

    public Payload getPayload() {
        return this.payload;
    }

    public void setPayload(Payload payload) {
        this.payload = payload;
    }

    public String toString(){
        return topic.toString() + "\n" + payload.toString();
    }

   
    
}
