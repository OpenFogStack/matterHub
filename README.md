# matterHub

(A)DSP: Lightweight matter Hub

## Setup

Run `source ./scripts/activate.sh`

## Build

for esp32:\
`cd src; rm sdkconfig; idf.py build`

or for m5-Stack:\
`cd src; rm sdkconfig; idf.py -D 'SDKCONFIG_DEFAULTS=sdkconfig_m5stack.defaults' build`

## References

[This tutorial on how to use idf.py](https://github.com/project-chip/connectedhomeip/tree/master/examples/all-clusters-app/esp32)
