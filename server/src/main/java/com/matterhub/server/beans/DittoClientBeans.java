package com.matterhub.server.beans;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Bean;
import org.springframework.stereotype.Component;


@Component
public class DittoClientBeans {

    @Autowired
    private MatterDittoClient client;

    @Bean
    public void runSubscription() {
        client.initializeDittoClient();
        client.subscribeToChanges();
    }

   
}