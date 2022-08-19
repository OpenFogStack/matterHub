package com.matterhub.server.entities.metrics;

import com.fasterxml.jackson.databind.JsonNode;
import lombok.ToString;

public record MetricDTO (String name, JsonNode value, String dataType, long timestamp) {
}
