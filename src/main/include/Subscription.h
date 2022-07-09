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
#include <InteractionModel.h>

typedef void (*SubscriptionCallback)(const chip::app::ConcreteDataAttributePath &, chip::TLV::TLVReader *, void * context);
class Subscription : public InteractionModelReports, public chip::app::ReadClient::Callback
{

public:
    Subscription(chip::DeviceProxy * device, chip::EndpointId endpointId, chip::ClusterId clusterId, chip::AttributeId attributeId,

                 uint16_t minInterval, uint16_t maxInterval, SubscriptionCallback callback = nullptr, void* context = nullptr) :
        InteractionModelReports(this),
        mDevice(device), mMinInterval(minInterval), mMaxInterval(maxInterval),mCallback(callback), mContext(context)
    {
        mEndpointId  = { endpointId };
        mClusterId   = { clusterId };
        mAttributeId = { attributeId };
    }

    Subscription(chip::DeviceProxy * device, chip::EndpointId endpointId, chip::ClusterId clusterId, chip::AttributeId attributeId,
                 SubscriptionCallback callback = nullptr, void* context = nullptr) :
        InteractionModelReports(this),
        mDevice(device), mMinInterval(1), mMaxInterval(10),mCallback(callback), mContext(context)

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

    //TODO: Cleanup after Read
    CHIP_ERROR Read() { return ReadAttribute(mDevice, mEndpointId, mClusterId, mAttributeId, chip::Optional<bool>(true)); }

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


        ESP_LOGI("Subscription", "This: %p", this);
        ESP_LOGI("Subscription", "mdevice: %p", mDevice);
        if (mCallback != nullptr)
        {
          if(mContext != nullptr){
            mCallback(path, data, mContext);
          }else{
            mCallback(path, data, this);
          }
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

    void OnDone(chip::app::ReadClient * apReadClient) override{
        Platform::Delete(this);
    };
    CHIP_ERROR mError = CHIP_NO_ERROR;
    SubscriptionCallback mCallback;
    void* mContext;
};

