# Command structure

How does a matter shell command get implemented

1. [Create Command Handler](https://github.com/OpenFogStack/matterHub/blob/77bc3f01889931f1c02213972e1cbad2a4b60971/src/main/BindingHandler.cpp#L234-L247)
2. [Add command to a shell_command_t[]](https://github.com/OpenFogStack/matterHub/blob/77bc3f01889931f1c02213972e1cbad2a4b60971/src/main/BindingHandler.cpp#L372)
3. [`Engine::Root().RegisterCommands(&sSwitchCommand, 1);`](https://github.com/OpenFogStack/matterHub/blob/77bc3f01889931f1c02213972e1cbad2a4b60971/src/main/BindingHandler.cpp#L385)

The handler then schedules work via [`DeviceLayer::PlatformMgr().ScheduleWork`](https://github.com/OpenFogStack/matterHub/blob/77bc3f01889931f1c02213972e1cbad2a4b60971/src/main/BindingHandler.cpp#L190)
the function takes a single argument which needs to be created with `Platform::New`.
This argument needs to be deleted with `Platform::Delete(data);`

For reading data, we need to have a look at the [chip-tool's ReportCommand](https://github.com/project-chip/connectedhomeip/blob/0507abb84dc43315738c050b79119fa643523699/examples/chip-tool/commands/clusters/ReportCommand.h#L26)