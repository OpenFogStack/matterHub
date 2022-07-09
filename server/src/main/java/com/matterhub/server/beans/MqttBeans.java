package com.matterhub.server.beans;

import javax.net.ssl.SSLSocketFactory;

import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.json.JSONObject;
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

import com.matterhub.cache.Cache;
import com.matterhub.server.entities.MessageType;
import com.matterhub.server.entities.Metric;
import com.matterhub.server.entities.MqttMatterMessage;
import com.matterhub.server.entities.Payload;
import com.matterhub.server.entities.Topic;

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
        MqttPahoMessageDrivenChannelAdapter adapter = new MqttPahoMessageDrivenChannelAdapter("PublisherId", mqttClientFactory(),
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
                System.out.println("Getting message from HiveMQ: ");
                System.out.println(message.getHeaders().get(MqttHeaders.RECEIVED_TOPIC).toString());
                System.out.println(message.getPayload());

                Topic topic = new Topic(message.getHeaders().get(MqttHeaders.RECEIVED_TOPIC).toString());
                Payload payload = new Payload(new JSONObject(message.getPayload().toString()), topic.getMessageType());
                Cache.put(new MqttMatterMessage(topic, payload));

                client.initializeDittoClient();
                if (topic.getMessageType() == null) {
                    return;
                }
                if (topic.getMessageType().equals(MessageType.DDATA)) {
                    client.setProps(topic.getThingId(), payload.getMetrics()[0].getAttribute(),
                        String.valueOf(payload.getMetrics()[0].getValue()), payload.getMetrics()[0].getCluster());
                    client.updateThing();
                } else if (topic.getMessageType().equals(MessageType.DBIRTH)) {
                    Metric onoffMetric = null;
                    for(var metric : payload.getMetrics()){
                        if(metric.getCluster().equals("on-off"))
                            onoffMetric = metric;
                    }
                    if(onoffMetric == null) return;
                    client.setProps(topic.getThingId(), onoffMetric.getAttribute(),
                        String.valueOf(onoffMetric.getValue()), onoffMetric.getCluster());
                    //TODO check if this works
                    client.createThing();
                }
            }

        };
    }

}