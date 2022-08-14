# Java Server

## First steps

1. First of all, you need an active [Subscription](https://accounts.bosch-iot-suite.com/subscriptions/) to the Bosch IoT Suite:
    - Click on new Subscription and select "Bosch IoT Device Management".
    - Choose the Plan you want and give your subscription an Instance Name.

2. You have to configure a new [OAuth2 Client](https://accounts.bosch-iot-suite.com/oauth2-clients/):
    - Click on New OAuth2 Client.
    - Write your Client Name, select all Options on the Client Scopes and select the Owner Option from the Organization Scopes.
    - Click on Create.

3. Update the application.properties. You will find the necessary credentials [here](https://accounts.bosch-iot-suite.com/oauth2-clients):
    - On the OAuth2 Page click on Use and then on Spring.
    - Copy the application.properties Part from "Define the configuration properties in application.properties:"
    - Insert the copied application.properties in src/main/resources/application.properties (you can edit the file with VSCode).

4. Insert the HiveMQ Password in application.properties in src/main/resources/application.properties
    - Contact the HiveMQ Admin, if you need the password.

5. Create a Namespace.
    - Go to [Subscriptions](https://accounts.bosch-iot-suite.com/subscriptions/) and choose "Go to Dashboard" and now choose "Things Dashboard".
    - Click on Namespace and "Create new namespace"
    - Choose a namespace name and click on Comfort and Create it.
    - Write the new created namespace in the "bosch-iot-suite.namespace"-field in application.properties in src/main/resources/application.properties

6. Copy the Test Access Token
    - Go to [OAuth2 Clients](https://accounts.bosch-iot-suite.com/oauth2-clients/)
    - Click on Use and copy the blue Test Access Token field.

7. Go to the [Bosch IoT API](https://apidocs.bosch-iot-suite.com/?urls.primaryName=Bosch%20IoT%20Things%20-%20HTTP%20API%20(v2)) and click on Authorize. Now you can insert the Test Access Token and click on Authorize.

8. Create a Thing via the [Bosch IoT API](https://apidocs.bosch-iot-suite.com/?urls.primaryName=Bosch%20IoT%20Things%20-%20HTTP%20API%20(v2))
    - Click on Things and then on PUT /thing/{thingId}
    - Click on Try it out.
    - In the first field write your thingId which means: your Namespace (from Step 6), your hubId(0) and your nodeId(333). \
      The thingId should follow this schema: ``` namespace:hubId_nodeId ```
    - On the request body you have to enter this JSON Code:

``` json
{
  "features": {
    "on-off": {
      "desiredProperties": {
        "on": 0
      }
    }
  }
}
```

### Solution ID

If you need a Solution ID, this is how you can find it:

- Go to [Subscriptions](https://accounts.bosch-iot-suite.com/subscriptions/) and choose "Go to Dashboard" and now choose "Things Dashboard".
- Now you can copy your Solution ID.

## Requirements

- Java (jdk-17)
- Maven (3.8.x)

## Run the server

Inside the server folder run
`mvn clean package`
and `java -jar target/server-0.0.1-SNAPSHOT.jar`

## Known bugs

There is a bug which throws an Exception every time a topic is published (org.eclipse.paho.client.mqttv3.internal.ExceptionHelper.createMqttException(ExceptionHelper.java:31)). It seems like this Exception has no impact on the correct server functionality. See <https://github.com/eclipse/paho.mqtt.android/issues/209>
