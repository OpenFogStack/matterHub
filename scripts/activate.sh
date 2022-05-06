#!/bin/bash
set -e
# call connectedhomeip setup script
pushd thirdparty/chip/repo
source ./scripts/activate.sh
popd
