# Demo

Script collection for device setup and commissioning

It has been developed on and for Linux, but adjusting it for MacOS should be possible.

## Dependencies

expect `sudo apt-get install expect`

(and if you want tmux) `sudo apt-get install tmux`

## Configuration

Before your first run you have to adjust the configuration (./config)

```bash
set MATTER_HUB_DIR "~/matter/matterHub"
set CHIP_TOOL_PATH "/home/max/matter/connectedhomeip/out/debug/chip-tool"
set SSID "YOUR SSID"
set PASSWORD "YOUR PASSWORD"
```

Adjust those four settings to your environment, if required you can also adjust the TTYs for the ESP32 and the M5 and maybe the shell (on your own risk)

You can ignore changes in the config file by using:

```bash
git update-index --skip-worktree scripts/demo/config
```

## Usage

### Terminals

`./demo.sh`

### Tmux

`./tmux.sh`
