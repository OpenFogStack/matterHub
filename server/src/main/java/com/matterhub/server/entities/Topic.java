package com.matterhub.server.entities;

public class Topic {

    private String namespace = null;
    private String groupId = null;
    private MessageType messageType;
    private String matterHubId;
    private String matterNodeId = null;
    private String thingId;

    public Topic(String namespace, String groupId, MessageType messageType, String matterHubId, String matterNodeId) {
        this.namespace = namespace;
        this.groupId = groupId;
        this.messageType = messageType;
        this.matterHubId = matterHubId;
        this.matterNodeId = matterNodeId;
        this.thingId = matterHubId + "_" + matterNodeId;
    }

    public Topic(MessageType messageType, String matterHubId, String matterNodeId) {
        this.messageType = messageType;
        this.matterHubId = matterHubId;
        this.matterNodeId = matterNodeId;
        this.thingId = matterHubId + "_" + matterNodeId;
    }
    
    
    public Topic(String namespace, String groupId, MessageType messageType, String matterHubId) {
        this.namespace = namespace;
        this.groupId = groupId;
        this.messageType = messageType;
        this.matterHubId = matterHubId;
    }

    public Topic(String topic) {
        String[] elements = topic.split("/");
        this.namespace = elements[0];
        this.groupId = elements[1];
        this.messageType =  MessageType.getMessageType(elements[2]);
        this.matterHubId = elements[3];
        if (elements.length == 5) {
            this.matterNodeId = elements[4];
        } 
        this.thingId = matterHubId + "_" + matterNodeId;
    }

    public String getMatterNodeId(){
        return this.matterNodeId;
    }

    public String getMatterHubId(){
        return this.matterHubId;
    }

    public String getThingId(){
        return this.thingId;
    }

    public MessageType getMessageType(){
        return this.messageType;
    }

    public String toString(){
        String topic = "";
        if (namespace == null && groupId == null) {
            topic += "spBv1.0/matterhub";
        } else {
            topic += namespace + "/" + groupId;
        }
        topic += "/" + messageType + "/" + matterHubId;
        if (matterNodeId!=null) {
            topic += "/" + matterNodeId;
        }
        return topic;
    }

    public boolean equals(Topic topic){
        if(thingId != null) {
            if (this.thingId.equals(topic.getMatterNodeId()) && this.matterHubId.equals(topic.getMatterHubId())) {
                return true;
            }
        }
        return false;
    }




    
    
}
