package com.matterhub.server.entities.matter;

import com.matterhub.server.entities.Metric;
import org.eclipse.ditto.json.JsonField;

public interface Attribute {
    // uint32_t See
    // https://github.com/project-chip/connectedhomeip/tree/master/src/lib/core/DataModelTypes.h#L30
    int Id();

    /**
     * Note that this name is also the one set as a key in {@see Attribute.toThingsRepresentation}
     * @return the human readable name of the Attribute
     */
    String Name();

    Cluster Parent();

    /**
     * Encodes the state as a command that will then be executed by the matterHub
     */
    Metric toMetric();

    /**
     * Parses the attributes internal representation
     * from a byte array that was sent by the matterHub
     */
    void fromMatterValue(byte[] matterValue);

    /**
     * This method serializes the attributes internal representation to be stored
     * as a property in the IOTT Cloud
     */
    JsonField toThingsRepresentation();

    /**
     * Parses the property coming from the IOTT Cloud
     */
    void fromThingsRepresentation(JsonField featureValue);
}
