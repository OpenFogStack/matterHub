package com.matterhub.server;

import com.matterhub.server.entities.matter.Matterhub;
import com.matterhub.server.entities.metrics.Metric;
import lombok.Synchronized;
import org.springframework.stereotype.Component;

import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

@Component
public class WorldState {

    private static WorldState state;

    private final List<Matterhub> mattterHubs;

    private WorldState() {
        mattterHubs = new ArrayList<>();
    }

    public static WorldState worldState() {
        if (state == null) {
            state = new WorldState();
        }
        return state;
    }

    private Optional<Matterhub> get(int id) {
        return mattterHubs.stream().filter(matterHub -> matterHub.Id() == id).findFirst();
    }

    private void addHub(Matterhub hub) {
        mattterHubs.add(hub);
    }

    synchronized public List<Metric> apply(Matterhub other) {
        return this.get(other.Id()).map(existing -> existing.apply(other)).orElseGet(() -> {
            this.addHub(other);
            return List.of();
        });
    }
}
