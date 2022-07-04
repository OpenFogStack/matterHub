SESSION=matterHUB
TIMEOUT=10
source ./config

$TERM -- bash -c "./chip-tool.exp; exec bash"
$TERM -- bash -c "./m5.exp; exec bash"
$TERM -- bash -c "./esp32.exp; exec bash"
