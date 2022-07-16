#!/bin/bash
cd $MATTERPATH
source ./scripts/activate.sh
cd src
idf.py build
idf.py flash monitor -p "${ENDDEVICE}"
