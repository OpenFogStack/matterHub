package com.matterhub.server.entities.payloads;

import com.matterhub.server.entities.MessageType;
import com.matterhub.server.entities.matter.Endpoint;
import com.matterhub.server.entities.metrics.DDataMetric;

import java.util.HashMap;
import java.util.List;

public final class DDataPayload extends Payload {
    List<DDataMetric> metrics;
    public DDataPayload(int sequenceNumber, long timestamp, List<DDataMetric> metrics) {
        super(sequenceNumber, timestamp);
        this.metrics = metrics;
    }

    @Override
    public MessageType getMessageType() {
        return MessageType.DDATA;
    }

    public List<Endpoint> getEndpoints() {
       HashMap<Short, Endpoint> kv = metrics.stream()
                                            .map(DDataMetric::toEndpoint)
                     .reduce(new HashMap<>(), (map, endpoint) -> {
                         if (!map.containsKey(endpoint.Id())) {
                             map.put(endpoint.Id(), endpoint);
                         }
                         map.get(endpoint.Id()).apply(endpoint);
                         return map;
                     }, (map1, map2) -> {
                         for (Short key : map2.keySet()) {
                             if (!map1.containsKey(key)) {
                                 map1.put(key, map2.get(key));
                             }
                             map1.get(key).apply(map2.get(key));
                         }
                         return map1;
                     });
       return kv.values().stream().toList();
    }
}
