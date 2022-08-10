package com.matterhub.server.entities.matter;


public interface Attribute {
    // uint32_t See https://github.com/project-chip/connectedhomeip/tree/master/src/lib/core/DataModelTypes.h#L30
    int Id();
    String Name();
    Cluster Parent();
}
