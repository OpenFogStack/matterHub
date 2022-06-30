/*
 *    Published under MIT License
 *    Copyright (c) 2022 OpenFogStack
 *    Based on code written by Project CHIP Authors, which was published under Apache License, Version 2.0 (c) 2022
 *    and the cJSON examples: https://github.com/nopnop2002/esp-idf-json, which are under Public Domain
 */
// TODO Source this
#pragma once

#include "BindingHandler.h"
#include "MQTTCommands.h"
#include "MQTTManager.h"
#include "cJSON.h"
#include "esp_log.h"
#include <Attributes.h>
#include <InteractionModel.h>

class Subscription : public InteractionModelReports, public chip::app::ReadClient::Callback
{

public:
    Subscription(chip::DeviceProxy * device, chip::FabricId fabricId, chip::EndpointId endpointId, chip::ClusterId clusterId,
                 chip::AttributeId attributeId, uint16_t minInterval, uint16_t maxInterval) :
        InteractionModelReports(this),
        mDevice(device), mfabricId(fabricId), mMinInterval(minInterval), mMaxInterval(maxInterval)
    {
        mEndpointId  = { endpointId };
        mClusterId   = { clusterId };
        mAttributeId = { attributeId };
    }
    CHIP_ERROR DoSubscribe()
    {
        return SubscribeAttribute(mDevice, mEndpointId, mClusterId, mAttributeId, mMinInterval, mMaxInterval,
                                  chip::Optional<bool>(true), chip::NullOptional, chip::NullOptional);
    }

    chip::DeviceProxy * mDevice;
    chip::FabricId mfabricId;
    std::vector<chip::EndpointId> mEndpointId;
    std::vector<chip::ClusterId> mClusterId;
    std::vector<chip::AttributeId> mAttributeId;
    uint16_t mMinInterval;
    uint16_t mMaxInterval;

private:
    // TODO: Create all required fields to create and manage a subscription

    /////////// ReadClient Callback Interface /////////
    void OnAttributeData(const chip::app::ConcreteDataAttributePath & path, chip::TLV::TLVReader * data,
                         const chip::app::StatusIB & status) override
    {

        CHIP_ERROR error = status.ToChipError();
        if (CHIP_NO_ERROR != error)
        {
            ChipLogError(chipTool, "Response Failure: %s", chip::ErrorStr(error));
            mError = error;
            return;
        }
        ESP_LOGI("Subscription", "EndpointId: 0x%02x", path.mEndpointId);
        ESP_LOGI("Subscription", "ClusterId: 0x%02x", path.mClusterId);
        ESP_LOGI("Subscription", "AttributeId: %d", path.mAttributeId);
        ESP_LOGI("Subscription", "NodeId: %llu", mDevice->GetDeviceId());
        ESP_LOGI("Subscription", "Fabric: %llu", mfabricId);

        if (data == nullptr)
        {
            ChipLogError(chipTool, "Response Failure: No Data");
            mError = CHIP_ERROR_INTERNAL;
            return;
        }

        // TODO: What we want to use:
        // https://github.com/project-chip/connectedhomeip/blob/cb116b9fba7f738e5efb52115d891c5094de1442/examples/chip-tool/templates/logging/DataModelLogger-src.zapt
        switch (path.mClusterId)
        {
        case chip::app::Clusters::OnOff::Id: {
            switch (path.mAttributeId)
            {
            case chip::app::Clusters::OnOff::Attributes::OnOff::Id: {
                bool value;
                CHIP_ERROR error = data->Get(value);
                if (error != CHIP_NO_ERROR)
                {
                    ESP_LOGE("Subscription", "Decoding error: %s", error.AsString());
                    return;
                }
                ESP_LOGI("Subscription", "OnOff: OnOff: %d", value);
                shell::MQTTCommandData * mqttCommand = chip::Platform::New<shell::MQTTCommandData>();
                /*
                std::string * topic                  = chip::Platform::New<std::string>(256, ' ');*/
                char * topic = (char *) chip::Platform::MemoryAlloc(sizeof(char) * 256);
                snprintf(topic, 256, "/spbv1.0/matterHub0/ddata/%llu/%llu/%u/%u", mfabricId, mDevice->GetDeviceId(),
                         path.mEndpointId, path.mClusterId);
                cJSON * root;
                root = cJSON_CreateObject();
                cJSON_AddNumberToObject(root, "timestamp", 13371337);
                cJSON * metrics;
                metrics = cJSON_AddArrayToObject(root, "metrics");
                cJSON * element;
                element = cJSON_CreateObject();
                cJSON_AddNumberToObject(element, "id", path.mAttributeId);
                cJSON_AddNumberToObject(element, "timestamp", 13371337);
                cJSON_AddStringToObject(element, "dataType", "Bool");
                cJSON_AddBoolToObject(element, "value", value);
                cJSON_AddItemToArray(metrics, element);
                char * my_json_string = cJSON_Print(root);
                mqttCommand->topic    = topic;
                mqttCommand->data     = my_json_string;
                mqttCommand->task     = shell::MQTTCommandTask::publish;

                chip::MQTTManager::GetInstance().ProcessCommand(mqttCommand);
            }
            break;
            default:
                ESP_LOGE("Subscription", "Unknown Attribute ID");
                break;
            }
        }
        break;

        default:
            ESP_LOGE("Subscription", "Unknown Cluster ID");
            break;
        }
    }

    void OnEventData(const chip::app::EventHeader & eventHeader, chip::TLV::TLVReader * data,
                     const chip::app::StatusIB * status) override
    {
        if (status != nullptr)
        {
            CHIP_ERROR error = status->ToChipError();
            if (CHIP_NO_ERROR != error)
            {
                ChipLogError(chipTool, "Response Failure: %s", chip::ErrorStr(error));
                mError = error;
                return;
            }
        }

        if (data == nullptr)
        {
            ChipLogError(chipTool, "Response Failure: No Data");
            mError = CHIP_ERROR_INTERNAL;
            return;
        }
    }
    void OnError(CHIP_ERROR error) override
    {
        ChipLogProgress(chipTool, "Error: %s", chip::ErrorStr(error));
        mError = error;
    }

    void OnDone(chip::app::ReadClient * apReadClient) override{};
    CHIP_ERROR mError = CHIP_NO_ERROR;
};
