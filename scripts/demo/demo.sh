SESSION=matterHUB
TIMEOUT=10
source ./config

gnome-terminal -- bash -c "./chip-tool.exp; exec bash"
gnome-terminal -- bash -c "./m5.exp; exec bash"
gnome-terminal -- bash -c "./esp32.exp; exec bash"
