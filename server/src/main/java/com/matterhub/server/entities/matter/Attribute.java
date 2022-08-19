package com.matterhub.server.entities.matter;

import com.fasterxml.jackson.databind.JsonNode;
import com.matterhub.server.entities.metrics.Metric;
import org.eclipse.ditto.json.JsonField;

import java.util.Optional;

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
     * Parses the attributes internal representation
     * from a JSON representation that was sent by the Matter hub
     */
    void fromMatterValue(JsonNode value);

    /**
     * This method serializes the attributes internal representation to be stored
     * as a property in the IOTT Cloud
     */
    JsonField toThingsRepresentation();

    /**
     * Parses the property coming from the IOTT Cloud
     * and emits a command that will set the Attribute
     * on the real device if required
     */
    Optional<Metric> fromThingsRepresentation(JsonField featureValue);
}
