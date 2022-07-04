## Java Server

Before usage please update application.properties. You will find the necessary credentials here: https://accounts.bosch-iot-suite.com/oauth2-clients. \
You will also need a device_management subscription and a namespace. \
Conntact the hivemq admin if you need the hivemq password. 

The easiest way to start is to create a thing via the Bosch API (PUT /thing/thingID). The thingID should follow this schema: namespace:hubId_nodeId \
Link to Bosch IoT API: https://apidocs.bosch-iot-suite.com/?urls.primaryName=Bosch%20IoT%20Things%20-%20HTTP%20API%20(v2)

### Requirements
- Java
- Maven

### Run the server
Inside the server folder run 
`mvn clean package`
and `java -jar target/server-0.0.1-SNAPSHOT.jar`

### Known bugs
There is a bug which throws an Exception every time a topic is published (org.eclipse.paho.client.mqttv3.internal.ExceptionHelper.createMqttException(ExceptionHelper.java:31)). It seems like this Exception has no impact on the correct server functionality. (https://github.com/eclipse/paho.mqtt.android/issues/209)
