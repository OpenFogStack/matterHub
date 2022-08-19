package com.matterhub.server.beans;

import java.util.concurrent.ExecutionException;

import org.eclipse.ditto.client.DittoClient;

public class Consumer extends Thread {

    private final DittoClient client;

    public Consumer(DittoClient client) {
        this.client = client;
    }


    @Override
    public void run() {
        while (!Thread.interrupted()) {
            try {
                client.twin().startConsumption().toCompletableFuture().get();
            } catch (InterruptedException | ExecutionException e) {
                e.printStackTrace();
            }
        }
    }

}
