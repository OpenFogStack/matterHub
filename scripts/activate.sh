#!/bin/bash

git submodule update --recursive
# call connectedhomeip setup script
pushd thirdparty/chip/repo || exit 1
source ./scripts/activate.sh
popd || exit 1
