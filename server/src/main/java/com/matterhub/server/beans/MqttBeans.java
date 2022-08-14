package com.matterhub.server.beans;

import javax.net.ssl.SSLSocketFactory;

import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.json.JSONObject;
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

import com.fasterxml.jackson.databind.JsonNode;
import com.matterhub.cache.Cache;
import com.matterhub.server.entities.MqttMatterMessage;
import com.matterhub.server.entities.Topic;
import com.matterhub.server.entities.matter.Cluster;
import com.matterhub.server.entities.matter.Endpoint;
import com.matterhub.server.entities.payloads.BasePayload;
import com.matterhub.server.entities.payloads.DBirthPayload;
import com.matterhub.server.entities.payloads.Payload;

@Configuration
public class MqttBeans {

    @Value("${hivemq.uri}")
    private String uri;

    @Value("${hivemq.user}")
    private String user;

    @Value("${hivemq.password}")
    private String password;

    @Autowired
    private MatterDittoClient client;

    private static final Logger LOGGER = LoggerFactory.getLogger(MqttBeans.class);

    @Bean
    public MqttPahoClientFactory mqttClientFactory() {
        DefaultMqttPahoClientFactory factory = new DefaultMqttPahoClientFactory();
        MqttConnectOptions options = new MqttConnectOptions();
        options.setServerURIs(new String[] { uri });
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
                Payload payload = Payload.parseMessage(topic.getMessageType(),
                        JsonNode(message.getPayload().toString()));
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
                Endpoint ep = payload.getClusters()
                client.createThing();
                Cluster onOffCluster = null;
                for (var cluster : payload.getClusters()) {
                    if (cluster.Name().equals("on-off"))
                        onOffCluster = cluster;
                }
                if (onOffCluster == null) {
                    return;
                }

                client.setProps(topic.getThingId(), onOffCluster.Name(),
                        String.valueOf(onOffCluster.getValue()), onOffCluster.Id());
                // TODO check if this works
                client.createThing();
            }

            private void handleBasePayload(Topic topic, BasePayload payload) {
                client.setProps(topic.getThingId(), payload.getMetrics()[0].getAttribute(),
                        String.valueOf(payload.getMetrics()[0].getValue()), payload.getMetrics()[0].getCluster());
                client.updateThing();
            }

        };
    }

}