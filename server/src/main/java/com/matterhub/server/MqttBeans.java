package com.matterhub.server;

import java.io.BufferedInputStream;
import java.io.FileInputStream;
import java.io.FileReader;
import java.security.KeyPair;
import java.security.KeyStore;
import java.security.Security;
import java.security.cert.CertificateFactory;
import java.security.cert.X509Certificate;
import java.util.Optional;

import javax.net.SocketFactory;
import javax.net.ssl.KeyManagerFactory;
import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLSocket;
import javax.net.ssl.SSLSocketFactory;
import javax.net.ssl.TrustManagerFactory;


import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.json.JSONArray;
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
    public MqttPahoClientFactory mqttClientFactory(){  
        DefaultMqttPahoClientFactory factory = new DefaultMqttPahoClientFactory();
        MqttConnectOptions options = new MqttConnectOptions();
        options.setServerURIs(new String [] {uri});
        options.setUserName(user);
        options.setPassword(password.toCharArray());
		options.setSocketFactory(SSLSocketFactory.getDefault());
        factory.setConnectionOptions(options);
        return factory;
    }

    @Bean
    public MessageChannel mqttChannel(){
        return new DirectChannel();
    }

    @Bean
    public MessageProducer inbound() {
        MqttPahoMessageDrivenChannelAdapter adapter = new MqttPahoMessageDrivenChannelAdapter("ID", mqttClientFactory(), "#");
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
				System.out.println(message.getHeaders().get(MqttHeaders.RECEIVED_TOPIC).toString());
				System.out.println(message.getPayload());

                String topic = message.getHeaders().get(MqttHeaders.RECEIVED_TOPIC).toString();
                // /spbv1.0/matterhubid/ddata/fabcricid/nodeid/endpointid/clusterid
                String [] data = topic.split("/");
                String fabric = data[4];
                String node = data[5];
                String endpoint = data[6];
                String cluster = data[7];
                String thingId = fabric + "_" + node + "_" + endpoint;
                String clusterMapping = "";
                //TODO implement mapping in config file
                if (cluster == "6") {
                    clusterMapping = "on-off";
                }

                JSONObject payload = new JSONObject(message.getPayload().toString());
                JSONArray metrics = payload.getJSONArray("metrics");
                //TODO add loop
                JSONObject attribute = metrics.getJSONObject(0);

                client.initializeDittoClient();
                client.setProps(thingId, attribute.getString("id"), attribute.getString("value"), clusterMapping);
                //TODO add handling if thing does not exist. Throw error or create new Thing?
                client.updateThing();
                client.destroy();
			}

		};
	}



}
