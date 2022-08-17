package com.matterhub.server.entities.matter;

import java.util.List;

public class Endpoint {
    short id;
    List<Cluster> clusters;
    Node parent;

    public Endpoint(short id, List<Cluster> clusters) {
        this.id = id;
        this.clusters = clusters;
        for (var cluster : clusters) {
            cluster.setParent(this);
        }
    }

    public List<Cluster> Clusters() {
        return this.clusters;
    };

    public short Id() {
        return this.id;
    }

    // TODO think about device ids
    // uint32_t See
    // https://github.com/project-chip/connectedhomeip/blob/master/src/lib/core/DataModelTypes.h#L36
    // List<Integer> DeviceIds();
    Node getParent() {
        return this.parent;
    };

    void setParent(Node parent) {
        this.parent = parent;
    }

    public String thingIdAString() {
        return parent.hub().id() + "_" + parent.nodeId() + "_" + this.id;
    }

}
