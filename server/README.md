# Java Server

## Requirements

- Java (jdk-17)
```
sudo apt install openjdk-17-jdk
```

- Maven (3.8.x)

Download the latest maven version from https://maven.apache.org/download.cgi

Extract the maven files:
```
sudo tar xf apache-maven-*.tar.gz -C /opt
```

Create a link:
```
sudo ln -s /opt/apache-maven-3.8.6 /opt/maven
```

Setup the Environment Variables:
in 

```
/etc/profile.d/maven.sh
```
add the following

```

export JAVA_HOME=/usr/lib/jvm/java-17-openjdk-amd64
export M2_HOME=/opt/maven
export MAVEN_HOME=/opt/maven
export PATH=${M2_HOME}/bin:${PATH} 
```

Make the script executable:
```
sudo chmod +x /etc/profile.d/maven.sh
```

Load the environment Variables:
```
source /etc/profile.d/maven.sh
```

Verify the setup:
```
mvn -version
```


## First steps

1. First of all, you need an active [Subscription](https://accounts.bosch-iot-suite.com/subscriptions/) to the Bosch IoT Suite:
    - If requried register a new Bosch-ID
    - Click on "new Subscription" and select "Bosch IoT Device Management".
    - Choose the Plan you want (free plan is fine) and give your subscription an Instance Name (Bosch recommends "unique" so maybe go with it?).
    - Click on "subscribe"

2. You have to configure a new [OAuth2 Client](https://accounts.bosch-iot-suite.com/oauth2-clients/):
    - Click on "New OAuth2 Client"
    - Write your Client Name
    - select *all* Options on the Client Scopes
    - select the "Owner" Option from the Organization Scopes
    - Click on "Create"

3. Update the application.properties. You will find the necessary credentials [here](https://accounts.bosch-iot-suite.com/oauth2-clients):
    - On the OAuth2 Page click on Use and then on Spring.
    - Copy the application.properties Part from "Define the configuration properties in application.properties:"
    - Insert the copied application.properties in src/main/resources/application.properties (you can edit the file with VSCode).

4. Insert the HiveMQ Password in application.properties in src/main/resources/application.properties
    - Contact the HiveMQ Admin, if you need the password.

5. Create a Namespace.
    - Go to [Subscriptions](https://accounts.bosch-iot-suite.com/subscriptions/) and choose "Go to Dashboard" and now choose "Things Dashboard".
    - Click on Namespace and "Create new namespace"
    - Choose a namespace name and click on "Comfort" and Create it.
    - Write the new created namespace in the "bosch-iot-suite.namespace"-field in application.properties in src/main/resources/application.properties

6. Copy the Test Access Token
    - Go to [OAuth2 Clients](https://accounts.bosch-iot-suite.com/oauth2-clients/)
    - Click on "Use" and copy the blue Test Access Token field.

7. Go to the [Bosch IoT API](https://apidocs.bosch-iot-suite.com/?urls.primaryName=Bosch%20IoT%20Things%20-%20HTTP%20API%20(v2)) and click on "Authorize" Now you can insert the Test Access Token and click on "Authorize".


8. Discover commissioned device: `matter discover describe 1 333` on the Matter Hub

9. Now you should be able to discover the newly created thing here:
https://apidocs.bosch-iot-suite.com/?urls.primaryName=Bosch%20IoT%20Things%20-%20HTTP%20API%20(v2)#/Things/get_things__thingId_

Enter the thingid: `<namespace>:<matterhub id>_<node id>_<endpoint id>`

For example:
`namespace:0_333_1`

It should look like this:
```
{
  "thingId": "namespace:0_333_1",
  "policyId": "namespace:0_333_1",
  "features": {
    "onoff": {
      "desiredProperties": {
        "OnOff": false,
        "GlobalSceneControl": false,
        "OnTime": 0,
        "StartUpOnOff": 0,
        "OffWaitTime": 0
      }
    }
  }
}
```

To change the thing you can send a [put request](https://apidocs.bosch-iot-suite.com/?urls.primaryName=Bosch%20IoT%20Things%20-%20HTTP%20API%20(v2)#/Things/put_things__thingId_)

enter again the thingid and a changed JSON representation of the thing:
```
{
  "thingId": "namespace:0_333_1",
  "policyId": "namespace:0_333_1",
  "features": {
    "onoff": {
      "desiredProperties": {
        "OnOff": true,
        "GlobalSceneControl": false,
        "OnTime": 0,
        "StartUpOnOff": 0,
        "OffWaitTime": 0
      }
    }
  }
}
```

Now the light should be turned on!

### Solution ID

If you need a Solution ID, this is how you can find it:

- Go to [Subscriptions](https://accounts.bosch-iot-suite.com/subscriptions/) and choose "Go to Dashboard" and now choose "Things Dashboard".
- Now you can copy your Solution ID.


## Run the server

Inside the server folder run
`mvn clean package`
and `java -jar target/server-0.0.1-SNAPSHOT.jar`

## Known bugs

There is a bug which throws an Exception every time a topic is published (org.eclipse.paho.client.mqttv3.internal.ExceptionHelper.createMqttException(ExceptionHelper.java:31)). It seems like this Exception has no impact on the correct server functionality. See <https://github.com/eclipse/paho.mqtt.android/issues/209>

## Resources:
https://phoenixnap.com/kb/install-maven-on-ubuntu
