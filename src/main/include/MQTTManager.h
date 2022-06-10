#pragma once

#include "MQTTCommands.h"

namespace chip {
class MQTTManager
{
public:
    MQTTManager(){};

    static MQTTManager & GetInstance()
    {
        if (!init)
        {
            initMQTTManager();
        }
        return sMQTTManager;
    }
    void SendPub(shell::MQTTCommandData * data);

private:
    static MQTTManager sMQTTManager;
    static void initMQTTManager();
    static bool init;
};
} // namespace chip