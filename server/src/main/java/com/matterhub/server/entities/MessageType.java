package com.matterhub.server.entities;

public enum MessageType {
    DBIRTH("DBIRTH"),
    DDATA("DDATA"),
    DCMD("DCMD"),
    NDATA("NDATA"),
    NCMD("NCMD"),
    DDEATH("DDEATH"),
    NDEATH("NDEATH");

    private String name;

    MessageType(String name) {
        this.name = name;
    }

    public String getName() {
        return name;
    }

    public static MessageType getMessageType(String name){
        switch (name) {
            case "DBIRTH":
                return MessageType.DBIRTH;
            case "DDATA":
                return MessageType.DDATA;
            case "DCMD":
                return MessageType.DCMD;
            case "NDATA":
                return MessageType.NDATA;
            case "NCMD":
                return MessageType.NCMD;
            case "DDEATH":
                return MessageType.DDEATH;
            case "NDEATH":
                return MessageType.NDEATH;
            default:
                return null;
        }
    }

}
