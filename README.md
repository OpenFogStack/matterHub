# matterHub

(A)DSP: Lightweight matter Hub

## Requirements
- PC with bluetooth hardware
- Wifi (which offers access to the internet)
- 2x ESP32

Bonus:
- Breadboard, 330 Ohm resistor and an LED

## Setup
### Prepare the Operating System
Install Ubuntu 20.04 LTS

- minimal version is enough
- update to the current version
- name your pc "John of us"

Install required software
```
sudo apt -y install git gcc g++ pkg-config libssl-dev libdbus-1-dev \
     libglib2.0-dev libavahi-client-dev ninja-build python3-venv python3-dev \
     python3-pip unzip libgirepository1.0-dev libcairo2-dev libreadline-dev expect

```
Choose a location and set the environment variable:
```
export MATTER_HUB_DIR="$HOME/matterHub/"
``` 

Clone this repository:
```
git clone https://github.com/OpenFogStack/matterHub.git ${MATTER_HUB_DIR}
```
change into the repository:
```
cd ${MATTER_HUB_DIR}
```
Initialize the project and matter:
```
source ${MATTER_HUB_DIR}/scripts/activate.sh
```
This will take some time...

build the matter repo:
```
cd ${MATTER_HUB_DIR}/thirdparty/chip/repo/
gn gen out/host
ninja -C out/host
```

### Configuration
move to the demo directory:
`cd ${MATTER_HUB_DIR}/scripts/demo`

copy the example config file:
`cp config.example config`

edit the example config with your favorite text editor:
`vi config`

replace YOUR MATTER HUB DIR with the path to the matter hub dir. If you cloned into your home directory it should look like this: 
```
set MATTER_HUB_DIR "~/matterHub"
```

replace "YOUR CHIP-TOOL PATH" with the path to the chip tool. if you followed this guid so far your chip-tool should be here:
```
set CHIP_TOOL_PATH "~/matterHub/thirdparty/chip/repo/out/host/chip-tool"
```

replace "SSID" and "YOUR PASSWORD" with your WiFi-SSID and your WiFi-password (sorry can help you here)

If you use the recommended setup (M5 Stack for the matterHub and an ESP32 for the lighting app) you can save and leave the file. 
Otherwise adjust the "M5_TTY" and "ESP32_TTY" entries accordingly.

### Configure MQTT:
####  Create HiveMQ Account and User
- Visit hiveMQ  https://console.hivemq.cloud/
- Sign up for a new account
- Confirm your E-Mail
- log in
- Enter further details 
- create a free cluster
- choose a cloud 
- create a new user:
    - manage cluster
    - access management
    - enter username and password: 
    - we will use:
        - Username: publicTest
        - Password: TODO:chooseABetterPassw0rd!
    - click "add"

Download a mqtt client to send commands directly to the Matter Hub. 
We used the hiveMQ cli: (https://github.com/hivemq/mqtt-cli/releases/)
But every other way to send mqtt messages to the topic should be fine.

#### Edit the matterHub configuration:
change into the source directory:
```
cd ${MATTER_HUB_DIR}/src/
```

Open menuconfig:
```
idf.py menuconfig
```

select "MQTT Configuration"

enter your HiveMQ credentials:

MQTT URI (you can find your under "overview"): it should look something like this:
```
mqtts://<some_prefix>.hivemq.cloud:8883 
```
Username and password are those you just chose.

Optional: 

consider setting you MatterHub ID Configuration, so you don't collide with other matterHubs

"MatterHUB ID Configuration"

Quit and save.


## Known Errors:
Connection timeout, Can not establish connection:
In our experience the WiFi module in the ESP32 is quite bad. So you should test as close to the WiFi-Hotspot as possbile (using the PC as a hotspot worked quite well too)



### Build ESP32 

Gain access to the serial ports:
```
sudo adduser $USER dialout
```
you probably need to reboot after this. 

After the reboot run the activate script again:

```
cd ${MATTER_HUB_DIR}
source scripts/activate.sh 
```

For the actual demo we provide a simple script:

```

cd ${MATTER_HUB_DIR}/scripts/demo/
./demo.sh
```

This should open three additional terminal windows:
- one used for commissioning the esp32 via the chip-tool
- one for flashing the lighting app on the esp32
- one for flashing the matterHub on the M5 stack

If everything works as designed you should see an MQTT_EVENT_CONNECTED on one of the shells
and No erros in the chip-tool shell.

If not possible causes of error:
- The Wifi is not reachable (yes the wifi chip of the esp32 is quite bad and for reliable communication you need to be quite close)
- Wifi password/user name is not correct
- MQTT has not been configured successfully

If all of the previous steps failed you can try to debug the situation by manually commissioning the devices (however we highly discourage you from doing this. This is a way into insanity. Trust me.)

#### Manual Configuration (USE ONLY IF THE DEMO SCRIPT DID NOT WORK)
you have been warned.

You again need three shells:
0. Setup:
    &emsp; $matterhub
    ```
    cd ${MATTER_HUB_DIR}/src
    ```
    
      &emsp; $chip-tool:
      ```
      cd ${MATTER_HUB_DIR}/thirdparty/chip/repo/out/host
          ```
          
        &emsp; $~/matterHub/thirdparty/chip/repo/examples/lighting-app/esp32
     ```
     cd ${MATTER_HUB_DIR}/thirdparty/chip/repo/examples/lighting-app/esp32
     ```
     
1. Cleanup:

    ``` bash
    idf.py erase-flash -p "${PORT_TO_M5}"
    idf.py erase-flash -p "${PORT_TO_ESP32}"
    rm -rf /tmp/*.ini /tmp/chip_*
    ```

2. Setup hub

    &emsp; $~/matterHub/src

    ```bash
    idf.py build
    idf.py flash monitor -p "${PORT_TO_M5}"
    ```

    &emsp; $chip-tool:

    ```bash
    chip-tool pairing ble-wifi 111 "${SSID}" "${PASSWORD}" 20202021 3840
    ```

3. Setup end-device:

    &emsp; $~/matterHub/thirdparty/chip/repo/examples/lighting-app/esp32

    ```bash
    idf.py build
    idf.py flash monitor -p "${PORT_TO_ESP32}"
    ```

    &emsp; $chip-tool:

    ```bash
    chip-tool pairing ble-wifi 333 "${SSID}" "${PASSWORD}" 20202021 3840
    ```

4. Give hub permission to access the end-device:

    ```bash
    chip-tool accesscontrol write acl '[{"fabricIndex": 1, "privilege": 5, "authMode": 2, "subjects": [112233], "targets": null },{"fabricIndex": 1, "privilege": 5, "authMode": 2, "subjects": [111], "targets": null }]' 333 0
    ```

If you want to use multiple end-devices repeat steps 3 and 4, and replace 333 with a different number.



Sidenote: A lot of the configuration (Comissioning, WiFi credentials, etc is stored permanently. Should something go wrong consider erasing the flash of the esp32 (idf.py erase-flash) and cleanup the data stored by the chip-tool (rm -rf /tmp/*.ini /tmp/chip_*)
The demo script will do this automaticly. 


Gratulations! You setup the ESP32 Part. 

## Java Server

### Requirements

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

# Start the server

Inside the server folder run
`mvn clean package`
and `java -jar target/server-0.0.1-SNAPSHOT.jar`

At this point you should have:
- configured the ESP32 setup
- been able to build the sources for the ESP32
- start the demo
- created and configured the hiveMQ Account
- created and configured the Bosch IoT Account
- been able to build and run the server

Congratulation, we can get started now.
# A simple Use-case

If not already happened: 
Build and start the server:
```
cd  ${MATTER_HUB_DIR}/server
mvn clean package
java -jar target/server-0.0.1-SNAPSHOT.jar
```

Start the demo:
```
cd ${MATTER_HUB_DIR}/scripts/demo/
./demo.sh
```


Discover commissioned device on the Matter Hub
`matter discover describe 1 333` 

Subscribe to an attribute (for example OnOff): 
```
mqtt pub -s -h <prefix>.s1.eu.hivemq.cloud -p 8883 -u publicTest -pw TODO:chooseABetterPassw0rd! --topic spBv1.0/matterhub/DCMD/0/333 -m '{"timestamp": 1234,"metrics": [{"name": "1/6/subscribe/0","timestamp": 1234}],"seq": 0}'

```

Verify that the discover was successful via the IoT API:

Copy the Test Access Token
    - Go to [OAuth2 Clients](https://accounts.bosch-iot-suite.com/oauth2-clients/)
    - Click on "Use" and copy the blue Test Access Token field.

Go to the [Bosch IoT API](https://apidocs.bosch-iot-suite.com/?urls.primaryName=Bosch%20IoT%20Things%20-%20HTTP%20API%20(v2)) and click on "Authorize" Now you can insert the Test Access Token and click on "Authorize".


Now you should be able to discover the newly created thing via [get_things](https://apidocs.bosch-iot-suite.com/?urls.primaryName=Bosch%20IoT%20Things%20-%20HTTP%20API%20(v2)#/Things/get_things__thingId_)
     - Click on "try it out"
     - Enter the thingid: `<namespace>:<matterhub id>_<node id>_<endpoint id>`
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

Currently only "OnOff" is supported.

Now the light should be turned on!

To verify the other direction you can either use a button to turn on the LED (see advanced use-case below) or use the MQTT Client

Turn toggle the light via MQTT:
```
mqtt pub -s -h b9fbe0bc6f56486fab5642f0b79f127e.s1.eu.hivemq.cloud -p 8883 -u publicTest -pw TODO:chooseABetterPassw0rd! --topic spBv1.0/matterhub/DCMD/0/333 -m '{"timestamp": 1234,"metrics": [{"name": "1/6/cmd/2","timestamp": 1234}],"seq": 0}'
```

Turn the light off: 

```
mqtt pub -s -h b9fbe0bc6f56486fab5642f0b79f127e.s1.eu.hivemq.cloud -p 8883 -u publicTest -pw TODO:chooseABetterPassw0rd! --topic spBv1.0/matterhub/DCMD/0/333 -m '{"timestamp": 1234,"metrics": [{"name": "1/6/cmd/0","timestamp": 1234}],"seq": 0}'
```

Turn the light on: 

```
mqtt pub -s -h b9fbe0bc6f56486fab5642f0b79f127e.s1.eu.hivemq.cloud -p 8883 -u publicTest -pw TODO:chooseABetterPassw0rd! --topic spBv1.0/matterhub/DCMD/0/333 -m '{"timestamp": 1234,"metrics": [{"name": "1/6/cmd/1","timestamp": 1234}],"seq": 0}'
```

## Known bugs

There is a bug which throws an Exception every time a topic is published (org.eclipse.paho.client.mqttv3.internal.ExceptionHelper.createMqttException(ExceptionHelper.java:31)). It seems like this Exception has no impact on the correct server functionality. See <https://github.com/eclipse/paho.mqtt.android/issues/209>


## Resources:
https://phoenixnap.com/kb/install-maven-on-ubuntu


## References

[This tutorial on how to use idf.py](https://github.com/project-chip/connectedhomeip/tree/master/examples/all-clusters-app/esp32)
