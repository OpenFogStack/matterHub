package com.matterhub.server.beans;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.matterhub.cache.Cache;
import com.matterhub.server.entities.matter.Matterhub;
import com.matterhub.server.entities.Metric;
import com.matterhub.server.entities.MqttMatterMessage;
import com.matterhub.server.entities.Topic;
import com.matterhub.server.entities.matter.Endpoint;
import com.matterhub.server.entities.matter.Node;
import com.matterhub.server.entities.payloads.BasePayload;
import com.matterhub.server.entities.payloads.DBirthPayload;
import com.matterhub.server.entities.payloads.Payload;
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
import java.util.List;

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
                Cache.put(new MqttMatterMessage(topic, payload));

                client.initializeDittoClient();
                if (topic.getMessageType() == null) {
                    return;
                }
                if (payload instanceof DBirthPayload) {
                    handleDBirthMessage(topic, (DBirthPayload) payload);
                } else {
                    handleBasePayload(topic, (BasePayload) payload);
                }

            }

            private void handleDBirthMessage(Topic topic, DBirthPayload payload) {
                List<Endpoint> eps = payload.getEndpoints();
                LOGGER.info("Topic {} contained endpoints {}", topic.getThingId(), eps);
                Matterhub hub = new Matterhub(topic.getMatterHubId());
                // This sets this node as the parent of the endpoints. DO NOT DELETE
                Node node = new Node(hub, topic.getMatterNodeId().orElseThrow(), eps);
                eps.forEach(ep -> client.createThing(ep));

            }

            private void handleBasePayload(Topic topic, BasePayload payload) {
                for (Metric m : payload.getMetrics()) {
                    client.updateThing(topic.getThingId(), m.getAttribute(), String.valueOf(m.getValue()), m.getCluster());
                }
            }
        };
    }

}