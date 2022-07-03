# matterHub

(A)DSP: Lightweight matter Hub

## Setup

Run `source ./scripts/activate.sh`

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

## References

[This tutorial on how to use idf.py](https://github.com/project-chip/connectedhomeip/tree/master/examples/all-clusters-app/esp32)
