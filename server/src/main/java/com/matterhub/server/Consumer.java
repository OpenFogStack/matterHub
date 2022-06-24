package com.matterhub.server;

import java.util.concurrent.ExecutionException;

import org.eclipse.ditto.client.DittoClient;

public class Consumer extends Thread {

    private DittoClient client;

    public Consumer(DittoClient client) {
        this.client = client;
    }

    @Override
    public void run() {
        try {
            client.twin().startConsumption().toCompletableFuture().get();
        } catch (InterruptedException | ExecutionException e) {
            e.printStackTrace();
        } 
    }
    
    
}
