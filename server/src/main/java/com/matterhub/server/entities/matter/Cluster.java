package com.matterhub.server.entities.matter;

import com.matterhub.server.entities.metrics.Metric;
import lombok.ToString;

import java.util.List;
import java.util.Set;
import java.util.stream.Stream;

@ToString
public abstract class Cluster {
    @ToString.Exclude private Endpoint parent;

    // uint32_t See https://github.com/project-chip/connectedhomeip/blob/e5e09f5c00b35c60debaabcf8f2a47f7f4a5ee91/src/lib/core/NodeId.h#L31
    public abstract int Id();
    public abstract String Name();
    public abstract Set<Attribute> Attributes();
    public abstract Set<Command> Commands();

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

    public Stream<Metric> apply(Cluster other) {
        assert this.Id() == other.Id();
        return other.Attributes().stream().flatMap(this::addOrApply);
    }
    private Stream<Metric> addOrApply(Attribute other) {
        assert this.Id() == other.Parent().Id();
        var maybeAttribute = this.Attributes()
                .stream().filter(n -> other.Id() == n.Id()).findFirst();
        if (maybeAttribute.isPresent()) {
            return maybeAttribute.get().fromThingsRepresentation(other.toThingsRepresentation()).stream();
        }
        Attribute otherNew = this.getAttribute(other.Id());
        this.Attributes().add(otherNew);
        return otherNew.fromThingsRepresentation(other.toThingsRepresentation()).stream();
    }
}
