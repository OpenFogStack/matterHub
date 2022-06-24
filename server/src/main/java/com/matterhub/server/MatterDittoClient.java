package com.matterhub.server;

import java.util.Arrays;
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
import org.eclipse.ditto.things.model.Feature;
import org.eclipse.ditto.things.model.FeatureProperties;
import org.eclipse.ditto.things.model.Thing;
import org.eclipse.ditto.things.model.ThingId;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Component;

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

        private DisconnectedDittoClient disconnectedDittoClient;

        private ThingId thingId;

        private Feature feature;

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

        public void setProps(String thingIdAsString, String path, String value, String cluster) {
                this.thingId = ThingId.of(namespace, thingIdAsString);
                FeatureProperties featureProperties = FeatureProperties.newBuilder().set(path, value).build();
                feature =  Feature.newBuilder().desiredProperties(featureProperties).withId(cluster).build();
        }

        private void update(DittoClient client) {
               
                client.twin().forId(thingId).putFeature(feature);
        }

        private void create(DittoClient client) {

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

        public void createThing() {
                disconnectedDittoClient.connect()
                                .thenAccept(this::create);
        }

        public void destroy(){
                disconnectedDittoClient.destroy();
        }

        public void subscribe(DittoClient client) {
                Consumer consumer = new Consumer(client);
                consumer.start();
                
                //TODO replace sysout with logger!
                System.out.println("Subscribed for Twin events");
                client.twin().registerForThingChanges("my-changes", change -> {
                        if (change.getAction() == ChangeAction.UPDATED) {
                                System.out.println("An existing Thing was modified: " + change.getThing());
                        }
                 }); 
        }

        public void subscribeToChanges() {
                disconnectedDittoClient.connect()
                                .thenAccept(this::subscribe);
        }
        

}