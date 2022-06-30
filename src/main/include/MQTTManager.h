/*
 *    Published under MIT License
 *    Copyright (c) 2022 OpenFogStack
 */
#pragma once

#include "MQTTCommands.h"
#include <vector>

namespace chip {
class MQTTManager
{
public:
    MQTTManager(){};

    static MQTTManager & GetInstance()
    {
        if (!mInit)
        {
            initMQTTManager();
            mInit = true;
        }
        return sMQTTManager;
    }
    void Publish(shell::MQTTCommandData * data);
    void Subscribe(shell::MQTTCommandData * data, std::function<void()> callback);
    void Unsubscribe(shell::MQTTCommandData * data);
    void ProcessCommand(shell::MQTTCommandData * data);

private:
    static MQTTManager sMQTTManager;
    void Subscribe(shell::MQTTCommandData * data);
    static void initMQTTManager();
    static bool mInit;
};
} // namespace chip
