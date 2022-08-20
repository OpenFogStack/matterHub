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
set MATTER_HUB_DIR="~"
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

Optional: change your matterHub Id:

Also consider setting you MatterHub ID Configuration, so you don't collide with other matterHubs.

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
scripts/demo/demo.sh
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

## Server:

## References

[This tutorial on how to use idf.py](https://github.com/project-chip/connectedhomeip/tree/master/examples/all-clusters-app/esp32)
