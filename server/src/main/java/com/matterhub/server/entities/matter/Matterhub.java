package com.matterhub.server.entities.matter;

import com.matterhub.server.entities.metrics.Metric;

import java.util.List;
import java.util.Set;
import java.util.stream.Stream;

public record Matterhub (int Id, Set<Node> nodes){

    /**
     * Applies the new state to the existing state tree.
     * If an attribute changes it will generate a command to set the state
     * on the matterHub.
     * Should the update being applied come from the matterHub, the list of commands can be discarded.
     * Once the other has been applied it will be in an invalid state and should no longer be used.
    */
    public List<Metric> apply(Matterhub other) {
        assert this.Id == other.Id;
        return other.nodes().stream().flatMap(this::addOrApply).toList();
    }

    private Stream<Metric> addOrApply(Node other) {
        assert this.Id == other.hub().Id;
        var maybeNode = nodes().stream().filter(n -> other.Id() == n.Id()).findFirst();
        if (maybeNode.isPresent()) {
            return maybeNode.get().apply(other);
        }
        // This breaks the `other` Node as its children no longer consider it the parent
        nodes.add(new Node(this, other.Id(), other.endpoints()));

        return Stream.empty();
    }
};
