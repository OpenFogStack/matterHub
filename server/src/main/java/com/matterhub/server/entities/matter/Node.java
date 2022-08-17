package com.matterhub.server.entities.matter;

import com.matterhub.server.entities.Matterhub;

import java.util.List;

public record Node(Matterhub hub,
                   // uint64_t See
                   // https://github.com/project-chip/connectedhomeip/blob/e5e09f5c00b35c60debaabcf8f2a47f7f4a5ee91/src/lib/core/NodeId.h#L29
                   long nodeId,
                   List<Endpoint> endpoints) {
    public Node {
        for (var endpoint : endpoints) {
            endpoint.setParent(this);
        }
    }
}
