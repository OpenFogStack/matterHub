package com.matterhub.server.entities.matter;

import java.util.List;

public abstract class Cluster {
    private Endpoint parent;

    // uint32_t See https://github.com/project-chip/connectedhomeip/blob/e5e09f5c00b35c60debaabcf8f2a47f7f4a5ee91/src/lib/core/NodeId.h#L31
    public abstract int Id();
    public abstract String Name();
    public abstract List<Attribute> Attributes();
    public abstract List<Command> Commands();

    public abstract Attribute getAttribute(int id);
    public abstract Attribute getAttribute(String name);

    public abstract Command getCommand(int id);
    public abstract Command getCommand(String name);

    
    public Endpoint Parent() {
        return parent;
    }

    public void setParent(Endpoint parent) {
        this.parent = parent;
    }
}
