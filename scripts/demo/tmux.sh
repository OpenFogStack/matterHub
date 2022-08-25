#!/usr/bin/env bash

SESSION=matterHUB
TIMEOUT=10
source ./config

function run_session(){
  SESSION=$1
  tmux new-session -s ${SESSION} -n matterHUB -d
  tmux select-layout main-horizontal
  # Cleanup
  tmux send-keys -t ${SESSION} "echo chip-tool" C-m
  tmux send-keys "./chip-tool.exp $ESP32_TTY $M5_TTY" C-m
  tmux set -p @mytitle "chip-tool"
  tmux split-window -t ${SESSION}:matterHUB
  tmux set -p @mytitle "M5"
  tmux send-keys "echo M5" C-m
  tmux send-keys "./m5.exp $MATTER_HUB_DIR $M5_TTY" C-m
  tmux split-window -h -t ${SESSION}:matterHUB
  tmux set -p @mytitle "ESP32"
  tmux send-keys "echo ESP-32" C-m
  tmux send-keys "./esp32.exp $LIGHTING_EXAMPLE_DIR $ESP32_TTY" C-m
  tmux select-window -t ${SESSION}:0
  tmux select-pane -t 1

}

# https://pityonline.gitbooks.io/tmux-productive-mouse-free-development_zh/content/book-content/Chapter3.html
if ! tmux has-session -t ${SESSION};
then
  run_session ${SESSION}
fi
tmux attach -t ${SESSION}
