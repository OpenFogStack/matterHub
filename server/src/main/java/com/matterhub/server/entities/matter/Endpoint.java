package com.matterhub.server.entities.matter;

import com.matterhub.server.entities.metrics.Metric;
import lombok.ToString;

import java.util.List;
import java.util.Set;
import java.util.stream.Stream;
@ToString
public class Endpoint {
    private final short id;
    private final Set<Cluster> clusters;
    @ToString.Exclude private Node parent;

    public Endpoint(short id, Set<Cluster> clusters) {
        this.id = id;
        this.clusters = clusters;
        for (var cluster : clusters) {
            cluster.setParent(this);
        }
    }

    public Endpoint(Node parent, short id, Set<Cluster> clusters) {
        this.id = id;
        this.clusters = clusters;
        for (var cluster : clusters) {
            cluster.setParent(this);
        }
        this.parent = parent;
        this.parent.endpoints().add(this);
    }

    public Set<Cluster> Clusters() {
        return this.clusters;
    }

    public short Id() {
        return this.id;
    }

    // TODO think about device ids
    // uint32_t See
    // https://github.com/project-chip/connectedhomeip/blob/master/src/lib/core/DataModelTypes.h#L36
    // List<Integer> DeviceIds();
    public Node Parent() {
        return this.parent;
    }

    void setParent(Node parent) {
        this.parent = parent;
    }

    public String thingIdAString() {
        return parent.hub().Id() + "_" + parent.Id() + "_" + this.id;
    }

    public Stream<Metric> apply(Endpoint other) {
        assert other.Id() == this.Id();
        return other.clusters.stream().flatMap(this::addOrApply);
    }

    private Stream<Metric> addOrApply(Cluster other) {
        assert this.Id() == other.Parent().Id();
        var maybeCluster = this.clusters.stream().filter(n -> other.Id() == n.Id()).findFirst();
        if (maybeCluster.isPresent()) {
            return maybeCluster.get().apply(other);
        }
        other.setParent(this);
        clusters.add(other);
        return Stream.empty();
    }
}
