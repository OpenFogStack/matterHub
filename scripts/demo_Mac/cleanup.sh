#!/bin/bash
BASEDIR=$(dirname "$0")
cd $BASEDIR;
source $BASEDIR/settings.sh
cd $MATTERPATH
source ./scripts/activate.sh
cd src
rm sdkconfig
idf.py build
idf.py erase_flash -p $HUB
idf.py erase_flash -p $ENDDEVICE
rm -rf /tmp/*.ini /tmp/chip_*