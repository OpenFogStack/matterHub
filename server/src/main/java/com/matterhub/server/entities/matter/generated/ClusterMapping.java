package com.matterhub.server.entities.matter.generated;

import java.util.List;

import com.matterhub.server.entities.matter.Cluster;
import com.matterhub.server.entities.matter.generated.onoff.OnOffCluster;

public class ClusterMapping {
    public static Cluster getClusterFromId(int id, List<Integer> attributeIds, List<Integer> commandIds) {
        switch (id) {
            case 6:
                return new OnOffCluster(attributeIds, commandIds);
            default:
                throw new IllegalArgumentException("Unkown cluster id: " + id);
        }
    }

    public static Cluster getClusterFromName(String name, List<Integer> attributeIds, List<Integer> commandIds) {
        switch (name) {
            case "on-off":
                return new OnOffCluster(attributeIds, commandIds);
            default:
                throw new IllegalArgumentException("Unkown cluster name: " + name);
        }
    }
}
