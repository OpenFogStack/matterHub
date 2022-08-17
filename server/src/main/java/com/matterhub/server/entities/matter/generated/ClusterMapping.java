package com.matterhub.server.entities.matter.generated;

import java.util.List;

import com.matterhub.server.entities.matter.Cluster;
import com.matterhub.server.entities.matter.generated.onoff.OnOffCluster;

public class ClusterMapping {
    public static Cluster getClusterFromId(int id, List<Integer> attributeIds, List<Integer> commandIds) {
        switch (id) {
            case 0x0006:
                return new OnOffCluster(attributeIds, commandIds, 0);
            default:
                throw new IllegalArgumentException("Unkown cluster id: " + id);
        }
    }

    public static Cluster getClusterFromName(String name, List<String> attributeNames, List<String> commandNames) {
        switch (name) {
            case "on-off":
                return new OnOffCluster(attributeNames, commandNames, "");
            default:
                throw new IllegalArgumentException("Unkown cluster name: " + name);
        }
    }
}
