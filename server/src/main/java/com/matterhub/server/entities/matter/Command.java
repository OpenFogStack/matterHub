package com.matterhub.server.entities.matter;


import com.matterhub.server.entities.metrics.Metric;

public interface Command {
    // uint32_t See https://github.com/project-chip/connectedhomeip/blob/e5e09f5c00b35c60debaabcf8f2a47f7f4a5ee91/src/lib/core/DataModelTypes.h#L33
    int Id();
    String Name();
    Cluster Parent();


    Metric toMetric();
}
