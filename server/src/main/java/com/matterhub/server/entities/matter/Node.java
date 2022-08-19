package com.matterhub.server.entities.matter;

import com.matterhub.server.entities.metrics.Metric;

import java.util.List;
import java.util.Objects;
import java.util.Set;
import java.util.stream.Stream;

public record Node(Matterhub hub,
                   // uint64_t See
                   // https://github.com/project-chip/connectedhomeip/blob/e5e09f5c00b35c60debaabcf8f2a47f7f4a5ee91/src/lib/core/NodeId.h#L29
                   long Id,
                   Set<Endpoint> endpoints) {
    public Node {
        hub.nodes().add(this);
        for (var endpoint : endpoints) {
            endpoint.setParent(this);
        }
    }
    public Stream<Metric> apply(Node other) {
        assert other.hub == this.hub;
        assert other.Id == this.Id;
        return other.endpoints().stream().flatMap(this::addOrApply);
    }

    private Stream<Metric> addOrApply(Endpoint other) {
        assert this.Id == other.Parent().Id();
        var maybeEndpoint = this.endpoints.stream().filter(n -> other.Id() == n.Id()).findFirst();
        if (maybeEndpoint.isPresent()) {
            return maybeEndpoint.get().apply(other);
        }
        other.setParent(this);
        endpoints.add(other);
        return Stream.empty();
    }
    @Override
    public String toString() {
        return "Node[ Id:" + Objects.toString(this.Id) + " Endpoints:" + Objects.toString(this.endpoints) + "]";
    }
}
