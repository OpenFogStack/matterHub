# matterHub

(A)DSP: Lightweight matter Hub

## Requirements
- PC with bluetooth hardware
- Wifi (which offers access to the internet)
- 2x ESP32

Bonus:
- Breadboard with 330k resistor and LED

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
Clone this repository:
```
git clone https://github.com/OpenFogStack/matterHub.git
```
change into the repository:
```
cd matterHub
```
Initialize the project and matter:
```
source ./scripts/activate.sh
```
This will take some time...

build the matter repo:
```
cd thirdparty/chip/repo/
gn gen out/host
ninja -C out/host
```

return to the base directory:
```cd ../../..```

### Create the required accounts:
#### HiveMQ
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


### Configuration
move to the demo directory:
`cd scripts/demo`

copy the example config file:
`cp config.example config`

edit the example config with your favorite text editor:
`vi config`

replace YOUR MATTER HUB DIR with the path to the matter hub dir (if you cloned into your home directory it should look like this: 
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

## Known Errors:
Connection timeout, Can not establish connection:
In our experience the WiFi module in the ESP32 is quite bad. So you should test as close to the WiFi-Hotspot as possbile (using the PC as a hotspot worked quite well too)



## Build

for esp32/m5-Stack:\
`cd src; idf.py build`

## Interaction Example

1. Cleanup:

    ``` bash
    idf.py erase-flash -p "${PORT_TO_HUB}"
    idf.py erase-flash -p "${PORT_TO_DEVICE}"
    rm -rf /tmp/*.ini /tmp/chip_*
    ```

2. Setup hub

    &emsp; $matterhub:

    ```bash
    idf.py build
    idf.py flash monitor -p "${PORT_TO_HUB}"
    ```

    &emsp; $chip-tool:

    ```bash
    chip-tool pairing ble-wifi 111 "${SSID}" "${PASSWORD}" 20202021 3840
    ```

3. Setup end-device:

    &emsp; $connectedhomeip/examples/lighting-app/esp32

    ```bash
    idf.py build
    idf.py flash monitor -p "${PORT_TO_DEVICE}"
    ```

    &emsp; $chip-tool:

    ```bash
    chip-tool pairing ble-wifi 333 "${SSID}" "${PASSWORD}" 20202021 3840
    ```

4. Give hub permission to access the end-device:

    ```bash
    chip-tool accesscontrol write acl '[{"fabricIndex": 1, "privilege": 5, "authMode": 2, "subjects": [112233], "targets": null },{"fabricIndex": 1, "privilege": 5, "authMode": 2, "subjects": [111], "targets": null }]' 333 0
    ```

5. Connect to end-device

    &emsp; $ Hub -> Shell (`matter switch binding unicast <fabric index> <node id> <endpoint>`):

    ```bash
    matter switch binding unicast 1 333 1
    ```

6. Switch LED:

    ```bash
    matter switch onoff on 1
    matter switch onoff off 1
    ```

## Optional 

### Configure MQTT

The MQTT options can be configured via:

idf.py menuconfig -> MQTT Configuration 

For example:
MQTT URI: mqtts://prefix.hivemq.cloud:8883
MQTT Username: user
MQTT Password: password

Also consider setting you MatterHub ID Configuration, so you don't collide with other matterHubs.

## References

[This tutorial on how to use idf.py](https://github.com/project-chip/connectedhomeip/tree/master/examples/all-clusters-app/esp32)
