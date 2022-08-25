#!/usr/bin/env bash
SESSION=matterHUB
TIMEOUT=10

screen -d -m bash -c "./chip-tool.exp; exec bash"
screen -d -m bash -c "./m5.exp; exec bash"
screen -d -m bash -c "./esp32.exp; exec bash"
