#!/bin/bash
BASEDIR=$(dirname -- "$( readlink -f -- "$0"; )")
cd $BASEDIR;
source $BASEDIR/settings.sh
cd $MATTERPATH
source ./scripts/activate.sh
cd src
idf.py build
idf.py erase_flash -p $HUB
idf.py erase_flash -p $ENDDEVICE
rm -rf /tmp/*.ini /tmp/chip_*
