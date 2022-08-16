package com.matterhub.server.beans;

import java.nio.ByteBuffer;
import java.util.Arrays;
import java.util.List;
import java.util.stream.Stream;

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
import org.eclipse.ditto.things.model.Feature;
import org.eclipse.ditto.things.model.FeatureProperties;
import org.eclipse.ditto.things.model.FeaturePropertiesBuilder;
import org.eclipse.ditto.things.model.Thing;
import org.eclipse.ditto.things.model.ThingId;
import org.eclipse.paho.client.mqttv3.IMqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.integration.mqtt.core.DefaultMqttPahoClientFactory;
import org.springframework.stereotype.Component;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.matterhub.server.entities.MessageType;
import com.matterhub.server.entities.Metric;
import com.matterhub.server.entities.Topic;
import com.matterhub.server.entities.matter.Attribute;
import com.matterhub.server.entities.matter.Cluster;
import com.matterhub.server.entities.matter.Endpoint;
import com.matterhub.server.entities.matter.generated.ClusterMapping;
import com.matterhub.server.entities.payloads.BasePayload;

@Component
public class MatterDittoClient {

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

    private static final Logger LOGGER = LoggerFactory.getLogger(MatterDittoClient.class);

    public void initializeDittoClient() {
        final ClientCredentialsAuthenticationConfiguration.ClientCredentialsAuthenticationConfigurationBuilder clientCredentialsAuthenticationConfigurationBuilder = ClientCredentialsAuthenticationConfiguration
                .newBuilder()
                .clientId(clientId)
                .clientSecret(clientSecret)
                .scopes(Arrays.asList(scope.split(" ")))
                .tokenEndpoint(accessTokenUri);

        final MessagingConfiguration.Builder messagingProviderConfigurationBuilder = WebSocketMessagingConfiguration
                .newBuilder()
                .jsonSchemaVersion(JsonSchemaVersion.V_2)
                .reconnectEnabled(false)
                .endpoint(wss);

        MessagingProvider messagingProvider = MessagingProviders.webSocket(
                messagingProviderConfigurationBuilder.build(),
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
        FeaturePropertiesBuilder builder = FeatureProperties
                .newBuilder();
        attributes.forEach(attribute -> builder.set(attribute.toThingsRepresentation()));
        return builder.build();
    }

    private void create(DittoClient client, Endpoint endpoint) {

        ThingId thingId = ThingId.of(namespace, endpoint.thingIdAString());

        final Thing thing = Thing.newBuilder()
                .setId(thingId)
                .build();
        final List<Feature> features = endpoint.Clusters().stream().map(cluster -> {
            FeatureProperties featureProperties = attributesToFeatureProperties(cluster.Attributes().stream());
            Feature feature = Feature
                    .newBuilder()
                    .desiredProperties(featureProperties)
                    .withId(cluster.Name())
                    .build();
            return feature;
        }).toList();
        ((ByteBuffer) client.twin().create(thing)
                .thenCompose(createdThing -> {
                    final Thing updatedThing = createdThing.toBuilder()
                            .setFeatures(features)
                            .build();
                    return client.twin().update(updatedThing);
                })).get();
    }

    /**
     * 
     * @param thingIdAsString
     * @param attributeName   - The human readable name of a given attribute
     * @param value           - The stringified value
     * @param clusterId
     */
    public void updateThing(String thingIdAsString, String attributeName, String value, String clusterId) {
        ThingId thingId = ThingId.of(namespace, thingIdAsString);
        if (attributeName.isEmpty()) {
            return;
        }
        FeatureProperties featureProperties = FeatureProperties.newBuilder().set(attributeName, value).build();
        Feature feature = Feature.newBuilder().desiredProperties(featureProperties).withId(clusterId)
                .build();
        disconnectedDittoClient.connect()
                .thenAccept(client -> this.update(client, thingId, feature));
    }

    public void createThing(Endpoint endpoint) {
        disconnectedDittoClient.connect()
                .thenAccept(client -> this.create(client, endpoint));
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

    private void handleChange(ThingChange change) {
        LOGGER.info(change.toString());
        try {
            if (!change.getThing().isPresent()) {
                LOGGER.warn("Changed thing no longer present");
                return;
            }
            long timestamp = change.getTimestamp().get().toEpochMilli();
            String[] entityId = change.getEntityId().toString().split(":");
            String[] thingId = entityId[1].split("_");
            String hubId = thingId[0];
            String nodeId = thingId[1];
            String endpointId = thingId[2];

            // TODO map this change back to attribute, so it can handle the conversion
            // Problem: Current interfaces assume that you always have the full list of
            // endpoints,
            // Clusters etc

            JsonObject features = (JsonObject) change.getValue().get();
            List<Cluster> changedClusters = parseChangeToCluster(features);
            Topic topic = new Topic(MessageType.DCMD, hubId, nodeId, endpointId);
            // TODO

            List<Metric> metrics = changedClusters
                    .stream()
                    .flatMap(cluster -> cluster.Attributes().stream())
                    .map(attr -> new Metric(attr.Parent().Id() + "/write/" + attr.Id(), timestamp, attr.toMatterValue(),
                            MessageType.DCMD))
                    .toList();

            BasePayload payload = new BasePayload(MessageType.DCMD, timestamp, metrics.toArray(new Metric[0]));

            sendPublish(topic, payload);
        } catch (Exception e) {
            LOGGER.error("Exception happened while handling change", e);
        }
    }

    private List<Cluster> parseChangeToCluster(JsonObject features) {
        return features
        .stream()
        .map(field -> {
            List<String> attributeNames = field.getValue().asObject().getKeys().stream().map(k-> k.toString()).toList();
            return ClusterMapping
            .getClusterFromName(field.getKeyName(), attributeNames, List.of());
        }).toList();

    }

    public void subscribeToChanges() {
        disconnectedDittoClient.connect()
                .thenAccept(this::subscribe);
    }

    public void sendPublish(Topic topic, BasePayload payload) throws JsonProcessingException {
        MqttMessage mqttMessage = new MqttMessage(payload.getPayload());
        mqttMessage.setQos(1);
        // FIXME We need the cache
        // this will be only false if the value has changed.
        // if (!Cache.checkIfMessageIsInCache(mqttMessage)) {
        //     LOGGER.info("Message found in cache. Not resending");
        //     return;
        // }
        LOGGER.info("Publishing message");
        DefaultMqttPahoClientFactory factory = new DefaultMqttPahoClientFactory();
        try (IMqttClient mqttClient = factory.getClientInstance(uri, "ID")) {

            MqttConnectOptions connOpts = new MqttConnectOptions();
            connOpts.setUserName(user);
            connOpts.setPassword(password.toCharArray());
            mqttClient.connect(connOpts);
            mqttClient.publish(topic.toString(), mqttMessage);
        } catch (MqttException e) {
            LOGGER.error("Error while sending message", e);
        }

    }

}
