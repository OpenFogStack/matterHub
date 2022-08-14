package com.matterhub.server.beans;

import java.util.Arrays;
import java.util.Optional;
import java.nio.ByteBuffer;

import org.eclipse.ditto.base.model.json.JsonSchemaVersion;
import org.eclipse.ditto.client.DisconnectedDittoClient;
import org.eclipse.ditto.client.DittoClient;
import org.eclipse.ditto.client.DittoClients;
import org.eclipse.ditto.client.changes.ChangeAction;
import org.eclipse.ditto.client.configuration.ClientCredentialsAuthenticationConfiguration;
import org.eclipse.ditto.client.configuration.MessagingConfiguration;
import org.eclipse.ditto.client.configuration.WebSocketMessagingConfiguration;
import org.eclipse.ditto.client.messaging.AuthenticationProviders;
import org.eclipse.ditto.client.messaging.MessagingProvider;
import org.eclipse.ditto.client.messaging.MessagingProviders;
import org.eclipse.ditto.json.JsonObject;
import org.eclipse.ditto.things.model.Feature;
import org.eclipse.ditto.things.model.FeatureProperties;
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
import com.fasterxml.jackson.databind.ObjectMapper;
import com.matterhub.cache.Cache;
import com.matterhub.server.entities.MessageType;
import com.matterhub.server.entities.Metric;
import com.matterhub.server.entities.Topic;
import com.matterhub.server.entities.matter.Endpoint;
import com.matterhub.server.entities.payloads.BasePayload;
import com.matterhub.server.entities.payloads.Payload;

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
    
    protected static final ObjectMapper OBJECT_MAPPER= new ObjectMapper();

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

    public void setProps(String thingIdAsString, String path, String value, Integer clusterId) {

        this.thingId = thingIdAsString;
        if (path.isEmpty()) {
            return;
        }
        FeatureProperties featureProperties = FeatureProperties.newBuilder().set(path, value).build();
        feature = Feature.newBuilder().desiredProperties(featureProperties).withId(clusterId.toString()).build();
    }

    private void update(DittoClient client) {
        if (thingId == null || feature == null) {
            return;
        }
        try {
            client.twin().forId(thingId).putFeature(feature);
        } catch (Exception e) {
            e.printStackTrace();
        }

    }

    private void create(DittoClient client, Endpoint endpoint) {

        ThingId thingId = ThingId.of(namespace, endpoint.thingIdAString());
        
        final Thing thing = Thing.newBuilder()
                .setId(thingId)
                .build();

        ((ByteBuffer) client.twin().create(thing)
                .thenCompose(createdThing -> {
                    final Thing updatedThing = createdThing.toBuilder()
                            .setFeature(feature)
                            .build();
                    return client.twin().update(updatedThing);
                })).get();
    }

    public void updateThing() {
        disconnectedDittoClient.connect()
                .thenAccept(this::update);
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
                LOGGER.info(change.toString());
                try {
                    if (!change.getThing().isPresent()) {
                        return;
                    }
                    long timestamp = change.getTimestamp().get().toEpochMilli();
                    String[] entityId = change.getEntityId().toString().split(":");
                    String[] thingId = entityId[1].split("_");
                    String hubId = thingId[0];
                    String nodeId = thingId[1];
                    String endpointId = thingId[2];

                    JsonObject features = (JsonObject) change.getValue().get();
                    String cluster = "on-off";
                    JsonObject attribute = JsonObject
                            .of(JsonObject.of(features.getValue("features").get().formatAsString()).getValue(cluster)
                                    .get().formatAsString());
                    JsonObject properties = (JsonObject) attribute
                            .getValue(attribute.getKeys().get(0))
                            .get();
                    String attributeId = properties.getKeys().get(0).toString();
                    String attributeValue = properties.getValue(attributeId).get().formatAsString();
                    Topic topic = new Topic(MessageType.DCMD, hubId, nodeId, endpointId);
                    // TODO
                    String name = endpointId + "/6/cmd/" + attributeValue;
                    Metric metric = new Metric(name, timestamp, attributeValue, MessageType.DCMD);
                    BasePayload payload = new BasePayload(MessageType.DCMD, timestamp, attributeValue,
                            metric);

                    // this will be only false if the value has changed.
                    if (!Cache.checkIfMessageIsInCache(hubId, Optional.of(nodeId), endpointId, cluster, attributeId,
                            attributeValue)) {
                        LOGGER.info("Message found in cache. Not resending");
                        return;
                    }
                    LOGGER.info("Publishing message");
                    sendPublish(topic, payload);
                } catch (Exception e) {
                    LOGGER.error("Exception happened while handling change", e);
                }

            }
        });
    }

    public void subscribeToChanges() {
        disconnectedDittoClient.connect()
                .thenAccept(this::subscribe);
    }

    public void sendPublish(Topic topic, Payload payload) throws JsonProcessingException {
        DefaultMqttPahoClientFactory factory = new DefaultMqttPahoClientFactory();
        try (IMqttClient mqttClient = factory.getClientInstance(uri, "ID")) {
            MqttMessage mqttMessage = new MqttMessage(payload.getPayload());
            mqttMessage.setQos(1);
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
