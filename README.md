# matterHub

(A)DSP: Lightweight matter Hub

## Setup

Run `source ./scripts/activate.sh`

## Build

for esp32:\
`cd src; rm sdkconfig; idf.py build`

or for m5-Stack:\
`cd src; rm sdkconfig; idf.py -D 'SDKCONFIG_DEFAULTS=sdkconfig_m5stack.defaults' build`

## Interaction Example

0. Cleanup:
```
idf.py erase_flash -p <port_to_hub>
idf.py erase_flash -p <port_to_device>
rm -rf /tmp/*.ini /tmp/chip_*
```
1. Setup hub 

&emsp; $matterhub: 
```
idf.py build
idf.py flash monitor -p <port_to_hub>
``` 
&emsp; $chip-tool: 
```
chip-tool pairing ble-wifi 111 "${SSID}" "${PASSWORD}" 20202021 3840
``` 

2. Setup end-device:

&emsp; $connectedhomeip/examples/lighting-app/esp32
```
idf.py build
idf.py flash monitor -p <port_to_device>
```
&emsp; $chip-tool: 
```
chip-tool pairing ble-wifi 333 "${SSID}" "${PASSWORD}" 20202021 3840
```

3. Give hub permission to access the end-device: 
```
chip-tool accesscontrol write acl '[{"fabricIndex": 1, "privilege": 5, "authMode": 2, "subjects": [112233], "targets": null },{"fabricIndex": 1, "privilege": 5, "authMode": 2, "subjects": [111], "targets": null }]' 333 0
```
4. Connect to end-device

&emsp; $ Hub -> Shell (`matter switch binding unicast <fabric index> <node id> <endpoint>`): 
```
matter switch binding unicast 1 333 1
```

5. Switch LED:
```
matter switch onoff on 1
matter switch onoff off 1
```



## References

[This tutorial on how to use idf.py](https://github.com/project-chip/connectedhomeip/tree/master/examples/all-clusters-app/esp32)
