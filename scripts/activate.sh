#!/bin/bash
IDF_VERSION="v4.4.1"
git submodule update --recursive

pushd thirdparty/esp-idf/repo || exit 1
git checkout ${IDF_VERSION}
git submodule update --init
./install.sh
. ./export.sh
popd || exit 1

# call connectedhomeip setup script
pushd thirdparty/chip/repo || exit 1
source ./scripts/activate.sh
popd || exit 1
