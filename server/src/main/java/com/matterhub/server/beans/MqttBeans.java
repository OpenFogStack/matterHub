package com.matterhub.server.beans;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.matterhub.server.WorldState;
import com.matterhub.server.entities.MessageType;
import com.matterhub.server.entities.Topic;
import com.matterhub.server.entities.matter.Cluster;
import com.matterhub.server.entities.matter.Endpoint;
import com.matterhub.server.entities.matter.Node;
import com.matterhub.server.entities.payloads.DBirthPayload;
import com.matterhub.server.entities.payloads.DDataPayload;
import com.matterhub.server.entities.payloads.Payload;
import com.matterhub.server.entities.payloads.PayloadDTO;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.integration.annotation.ServiceActivator;
import org.springframework.integration.channel.DirectChannel;
import org.springframework.integration.core.MessageProducer;
import org.springframework.integration.mqtt.core.DefaultMqttPahoClientFactory;
import org.springframework.integration.mqtt.core.MqttPahoClientFactory;
import org.springframework.integration.mqtt.inbound.MqttPahoMessageDrivenChannelAdapter;
import org.springframework.integration.mqtt.support.DefaultPahoMessageConverter;
import org.springframework.integration.mqtt.support.MqttHeaders;
import org.springframework.messaging.Message;
import org.springframework.messaging.MessageChannel;
import org.springframework.messaging.MessageHandler;
import org.springframework.messaging.MessagingException;

import javax.net.ssl.SSLSocketFactory;
import java.util.HashSet;
import java.util.Set;

@Configuration
public class MqttBeans {

    private static final Logger LOGGER = LoggerFactory.getLogger(MqttBeans.class);
    private static final ObjectMapper OBJECT_MAPPER = new ObjectMapper();
    @Value("${hivemq.uri}")
    private String uri;
    @Value("${hivemq.user}")
    private String user;
    @Value("${hivemq.password}")
    private String password;
    @Autowired
    private MatterDittoClient client;

    @Autowired
    private WorldState worldState;

    @Bean
    public MqttPahoClientFactory mqttClientFactory() {
        DefaultMqttPahoClientFactory factory = new DefaultMqttPahoClientFactory();
        MqttConnectOptions options = new MqttConnectOptions();
        options.setServerURIs(new String[]{uri});
        options.setUserName(user);
        options.setPassword(password.toCharArray());
        options.setSocketFactory(SSLSocketFactory.getDefault());
        options.setAutomaticReconnect(true);
        options.setCleanSession(true);
        options.setConnectionTimeout(30);
        factory.setConnectionOptions(options);
        return factory;
    }

    @Bean
    public MessageChannel mqttChannel() {
        return new DirectChannel();
    }

    @Bean
    public MessageProducer inbound() {
        MqttPahoMessageDrivenChannelAdapter adapter = new MqttPahoMessageDrivenChannelAdapter("PublisherId",
                                                                                              mqttClientFactory(),
                                                                                              "#");
        adapter.setConverter(new DefaultPahoMessageConverter());
        adapter.setOutputChannel(mqttChannel());
        return adapter;
    }

    @Bean
    @ServiceActivator(inputChannel = "mqttChannel")
    public MessageHandler handler() {
        return new MessageHandler() {

            @Override
            public void handleMessage(Message<?> message) throws MessagingException {
                LOGGER.info("Getting message from HiveMQ\n Topic: {} \n Body: {}",
                            message.getHeaders().get(MqttHeaders.RECEIVED_TOPIC).toString(), message.getPayload());

                Topic topic = new Topic(message.getHeaders().get(MqttHeaders.RECEIVED_TOPIC).toString());
                Payload payload;
                try {
                    payload = Payload.parseMessage(topic.getMessageType(),
                                                   OBJECT_MAPPER.readTree(message.getPayload().toString()));
                } catch (JsonProcessingException | IllegalArgumentException e) {
                    LOGGER.error("Unable to parse message", e);
                    throw new MessagingException(message, e);
                }

                client.initializeDittoClient();
                if (topic.getMessageType() == null) {
                    return;
                }
                if (payload instanceof DBirthPayload) {
                    handleDBirthMessage(topic, (DBirthPayload) payload);
                } else if (payload instanceof DDataPayload) {
                    handleDDataMessage(topic, (DDataPayload) payload);
                } else {
                    handleBasePayload(topic, (PayloadDTO) payload);
                }

            }

            private void handleDBirthMessage(Topic topic, DBirthPayload payload) {
                assert topic.getMessageType() == MessageType.DBIRTH;
                Set<Endpoint> eps = new HashSet<>(payload.getEndpoints());
                LOGGER.info("Topic {} contained endpoints {}", topic.getThingId(), eps);
                Node topicNode = topic.toNode().orElseThrow();

                // This sets this node as the parent of the endpoints. DO NOT DELETE
                Node node = new Node(topicNode.hub(), topicNode.Id(), eps);
                worldState.apply(node.hub());
                eps.forEach(ep -> client.createThing(ep));

            }

            private void handleDDataMessage(Topic topic, DDataPayload payload) {
                assert topic.getMessageType() == MessageType.DDATA;
                Set<Endpoint> endpoints = new HashSet<>(payload.getEndpoints());
                LOGGER.info("Topic {} contained clusters {}", topic.getThingId(), endpoints);
                Node topicNode = topic.toNode().orElseThrow();

                // This sets this node as the parent of the endpoints. DO NOT DELETE
                Node node = new Node(topicNode.hub(), topicNode.Id(), endpoints);
                worldState.apply(node.hub());
                endpoints.forEach(endpoint -> client.updateThing(endpoint));
            }

            private void handleBasePayload(Topic topic, PayloadDTO payload) {
                LOGGER.error("Unhandled topic {}", topic);
            }
        };
    }

}