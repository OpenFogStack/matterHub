#!/bin/bash
cd $MATTERPATH/thirdparty/chip/repo
rm -rf out
./scripts/examples/gn_build_example.sh examples/chip-tool out/
./out/chip-tool pairing ble-wifi 111 "${SSID}" "${PASSWORD}" 20202021 3840
./out/chip-tool pairing ble-wifi 333 "${SSID}" "${PASSWORD}" 20202021 3840
./out/chip-tool accesscontrol write acl '[{"fabricIndex": 1, "privilege": 5, "authMode": 2, "subjects": [112233], "targets": null },{"fabricIndex": 1, "privilege": 5, "authMode": 2, "subjects": [111], "targets": null }]' 333 0