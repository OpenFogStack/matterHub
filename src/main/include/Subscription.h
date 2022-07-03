/**
 *
 */
// TODO Source this
#pragma once

#include "BindingHandler.h"
#include "esp_log.h"
#include <InteractionModel.h>

typedef void (*SubscriptionCallback)(const chip::app::ConcreteDataAttributePath &, chip::TLV::TLVReader *);

class Subscription : public InteractionModelReports, public chip::app::ReadClient::Callback
{

public:
    Subscription(chip::DeviceProxy * device, chip::EndpointId endpointId, chip::ClusterId clusterId, chip::AttributeId attributeId,
                 uint16_t minInterval, uint16_t maxInterval, SubscriptionCallback callback = nullptr) :
        InteractionModelReports(this),
        mDevice(device), mMinInterval(minInterval), mMaxInterval(maxInterval), mCallback(callback)
    {
        mEndpointId  = { endpointId };
        mClusterId   = { clusterId };
        mAttributeId = { attributeId };
    }

    Subscription(chip::DeviceProxy * device, chip::EndpointId endpointId, chip::ClusterId clusterId, chip::AttributeId attributeId,
                 SubscriptionCallback callback = nullptr) :
        InteractionModelReports(this),
        mDevice(device), mMinInterval(1), mMaxInterval(10), mCallback(callback)
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

    CHIP_ERROR Read() { return ReadAttribute(mDevice, mEndpointId, mClusterId, mAttributeId, chip::Optional<bool>(true)); }

    chip::DeviceProxy * mDevice;
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

        if (data == nullptr)
        {
            ChipLogError(chipTool, "Response Failure: No Data");
            mError = CHIP_ERROR_INTERNAL;
            return;
        }

        chip::TLV::TLVType type = data->GetType();
        if (type == chip::TLV::TLVType::kTLVType_Boolean)
        {
            ESP_LOGI("Subscription", " - Attribute ID: '0x%02x'", path.mAttributeId);
            ESP_LOGI("Subscription", " - Attribute Type: '0x%02x'", type);
            bool on;
            if (data->Get(on) == CHIP_NO_ERROR)
                ESP_LOGI("Subscription", " - Attribute Value: '%s'", on ? "on" : "off");
            else
                ESP_LOGI("Subscription", "Error while reading attribute value");
        }

        if (mCallback != nullptr)
        {
            mCallback(path, data);
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
    SubscriptionCallback mCallback;
};