package com.matterhub.server.beans;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.matterhub.server.WorldState;
import com.matterhub.server.entities.MessageType;
import com.matterhub.server.entities.Topic;
import com.matterhub.server.entities.matter.*;
import com.matterhub.server.entities.matter.generated.ClusterMapping;
import com.matterhub.server.entities.metrics.Metric;
import com.matterhub.server.entities.metrics.MetricDTO;
import com.matterhub.server.entities.payloads.PayloadDTO;
import lombok.NonNull;
import org.eclipse.ditto.base.model.json.JsonSchemaVersion;
import org.eclipse.ditto.client.DisconnectedDittoClient;
import org.eclipse.ditto.client.DittoClient;
import org.eclipse.ditto.client.DittoClients;
import org.eclipse.ditto.client.changes.ChangeAction;
import org.eclipse.ditto.client.changes.ThingChange;
import org.eclipse.ditto.client.configuration.ClientCredentialsAuthenticationConfiguration;
import org.eclipse.ditto.client.configuration.MessagingConfiguration;
import org.eclipse.ditto.client.configuration.WebSocketMessagingConfiguration;
import org.eclipse.ditto.client.messaging.AuthenticationProviders;
import org.eclipse.ditto.client.messaging.MessagingProvider;
import org.eclipse.ditto.client.messaging.MessagingProviders;
import org.eclipse.ditto.json.JsonObject;
import org.eclipse.ditto.things.model.*;
import org.eclipse.paho.client.mqttv3.IMqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.integration.mqtt.core.DefaultMqttPahoClientFactory;
import org.springframework.stereotype.Component;

import java.util.Arrays;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.stream.Stream;

@Component
public class MatterDittoClient {

    private static final Logger LOGGER = LoggerFactory.getLogger(MatterDittoClient.class);
    @Value("${bosch-iot-suite-auth.oauth2.client.clientId}")
    private String clientId;
    @Value("${bosch-iot-suite-auth.oauth2.client.clientSecret}")
    private String clientSecret;
    @Value("${bosch-iot-suite-auth.oauth2.client.scope}")
    private String scope;
    @Value("${bosch-iot-suite-auth.oauth2.client.accessTokenUri}")
    private String accessTokenUri;
    @Value("${bosch-iot-suite.namespace}")
    private String namespace;
    @Value("${bosch-iot-suite.wss}")
    private String wss;
    @Value("${hivemq.uri}")
    private String uri;
    @Value("${hivemq.user}")
    private String user;
    @Value("${hivemq.password}")
    private String password;
    private DisconnectedDittoClient disconnectedDittoClient;

    @Autowired
    private WorldState worldState;

    public void initializeDittoClient() {
        final ClientCredentialsAuthenticationConfiguration.ClientCredentialsAuthenticationConfigurationBuilder
                clientCredentialsAuthenticationConfigurationBuilder =
                ClientCredentialsAuthenticationConfiguration.newBuilder()
                                                            .clientId(clientId)
                                                            .clientSecret(clientSecret)
                                                            .scopes(Arrays.asList(scope.split(" ")))
                                                            .tokenEndpoint(accessTokenUri);

        final MessagingConfiguration.Builder messagingProviderConfigurationBuilder =
                WebSocketMessagingConfiguration.newBuilder()
                                               .jsonSchemaVersion(JsonSchemaVersion.V_2)
                                               .reconnectEnabled(false)
                                               .endpoint(wss);

        MessagingProvider messagingProvider =
                MessagingProviders.webSocket(messagingProviderConfigurationBuilder.build(),
                                             AuthenticationProviders.clientCredentials(
                                                     clientCredentialsAuthenticationConfigurationBuilder.build()));
        this.disconnectedDittoClient = DittoClients.newInstance(messagingProvider);
    }

    private void update(DittoClient client, ThingId thingId, Feature feature) {
        if (thingId == null || feature == null) {
            LOGGER.error("Called update without setting thingId or feature");
            return;
        }
        try {
            client.twin().forId(thingId).putFeature(feature);
        } catch (Exception e) {
            LOGGER.error("Couldn't update " + thingId, e);
        }

    }

    private FeatureProperties attributesToFeatureProperties(Stream<Attribute> attributes) {
        FeaturePropertiesBuilder builder = FeatureProperties.newBuilder();
        attributes.forEach(attribute -> builder.set(attribute.toThingsRepresentation()));
        return builder.build();
    }

    private void create(DittoClient client, Endpoint endpoint) {
        ThingId thingId = ThingId.of(namespace, endpoint.thingIdAString());
        LOGGER.info("Starting to create {}", thingId);
        final Thing thing;
        final List<Feature> features;
        try {
            thing = Thing.newBuilder().setId(thingId).build();
            features = endpoint.Clusters().stream().map(cluster -> {
                return clusterToFeature(cluster);
            }).toList();
        } catch (Exception e) {
            LOGGER.error("Something went wrong ", e);
            throw e;
        }


        LOGGER.info("Thing has been built {}", thing);

        client.twin().create(thing).thenCompose(createdThing -> {
            final Thing updatedThing = createdThing.toBuilder().setFeatures(features).build();
            return client.twin().update(updatedThing);
        }).thenRun(() -> LOGGER.info("Finished creating {}", endpoint.thingIdAString())).exceptionally(e -> {
            LOGGER.error("Couldn't create thing", e);
            return null;
        });
    }

    private Feature clusterToFeature(Cluster cluster) {
        FeatureProperties featureProperties = attributesToFeatureProperties(cluster.Attributes().stream());
        return Feature.newBuilder().desiredProperties(featureProperties).withId(cluster.Name()).build();
    }

    /**
     * @param endpoint - The new value of the endpoint
     */
    public void updateThing(@NonNull Endpoint endpoint) {
        ThingId thingId = ThingId.of(namespace, endpoint.thingIdAString());
        endpoint.Clusters().stream().map(this::clusterToFeature)
                .forEach(feature -> disconnectedDittoClient
                        .connect()
                        .thenAccept(client -> this.update(client, thingId, feature)
                        ));
    }

    public void createThing(Endpoint endpoint) {
        disconnectedDittoClient.connect().thenAccept(client -> this.create(client, endpoint));
    }

    public void destroy() {
        disconnectedDittoClient.destroy();
    }

    private void subscribe(DittoClient client) {
        Consumer consumer = new Consumer(client);
        consumer.start();

        // refactor
        LOGGER.info("Subscribed for Twin events");
        client.twin().registerForThingChanges("my-changes", change -> {
            if (change.getAction() == ChangeAction.UPDATED) {
                handleChange(change);
            }
        });
    }

    public void handleChange(ThingChange change) {
        LOGGER.info(change.toString());
        try {
            if (change.getThing().isEmpty()) {
                LOGGER.warn("Changed thing no longer present");
                return;
            }
            String[] entityId = change.getEntityId().toString().split(":");
            String[] thingId = entityId[1].split("_");

            Matterhub hub = new Matterhub(Integer.parseInt(thingId[0]), new LinkedHashSet<>());
            Node node = new Node(hub, Long.parseLong(thingId[1]), new LinkedHashSet<>());
            Endpoint ep = new Endpoint(node, Short.parseShort(thingId[2]), new LinkedHashSet<>());
            node.endpoints().add(ep);
            JsonObject jChange = (JsonObject) change.getValue().get();
            List<Cluster> changedClusters =
                    parseChangeToCluster(ep, jChange.getValue("features").orElseThrow().asObject());
            ep.Clusters().addAll(changedClusters);
            List<Metric> metrics = worldState.apply(hub);
            if (metrics.isEmpty()) {
                LOGGER.info("No metrics generated");
                return;
            }
            Topic topic = new Topic(MessageType.DCMD, hub.Id(), node.Id());

            long timestamp = change.getTimestamp().orElseThrow().toEpochMilli();
            PayloadDTO payload = new PayloadDTO(MessageType.DCMD,
                                                timestamp,
                                                metrics.stream().map(Metric::toDTO).toArray(MetricDTO[]::new));
            LOGGER.info("Generated change payload {}", payload);
            sendPublish(topic, payload);
        } catch (Exception e) {
            LOGGER.error("Exception happened while handling change", e);
        }
    }

    public List<Cluster> parseChangeToCluster(Endpoint parent, JsonObject features) {
        // Features
        return features.stream().map(field -> {
            // Create the attributes
            JsonObject inner = field.getValue().asObject().getValue("desiredProperties").orElseThrow().asObject();
            List<String> attributeNames = inner.getKeys().stream().map(Object::toString).toList();
            Cluster cluster = ClusterMapping.getClusterFromName(field.getKeyName(), attributeNames, List.of());
            cluster.setParent(parent);
            // Set their values
            attributeNames.forEach(name -> {
                Attribute attr = cluster.Attributes()
                                        .stream()
                                        .filter(attribute -> attribute.Name().equals(name))
                                        .findFirst()
                                        .orElseThrow();
                attr.fromThingsRepresentation(inner.getField(name).orElseThrow());
            });
            return cluster;
        }).toList();

    }

    public void subscribeToChanges() {
        disconnectedDittoClient.connect().thenAccept(this::subscribe);
    }

    public void sendPublish(Topic topic, PayloadDTO payload) throws JsonProcessingException, MqttException {
        MqttMessage mqttMessage = new MqttMessage(payload.toPayload());
        mqttMessage.setQos(1);
        LOGGER.info("Publishing message");
        DefaultMqttPahoClientFactory factory = new DefaultMqttPahoClientFactory();
        try (IMqttClient mqttClient = factory.getClientInstance(uri, "ID")) {

            MqttConnectOptions connOpts = new MqttConnectOptions();
            connOpts.setUserName(user);
            connOpts.setPassword(password.toCharArray());
            mqttClient.connect(connOpts);
            mqttClient.publish(topic.toString(), mqttMessage);
        }

    }

}
