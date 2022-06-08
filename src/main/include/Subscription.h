/**
 *
 */
// TODO Source this
#pragma once

#include "BindingHandler.h"
#include <InteractionModel.h>

class Subscription : public InteractionModelReports, public chip::app::ReadClient::Callback
{

public:
    Subscription(chip::DeviceProxy * device, chip::EndpointId endpointId, chip::ClusterId clusterId, chip::AttributeId attributeId,
                 uint16_t minInterval, uint16_t maxInterval) :
        InteractionModelReports(this),
        mDevice(device), mEndpointId({endpointId}), mClusterId({clusterId}), mAttributeId({attributeId}), mMinInterval(minInterval),
        mMaxInterval(maxInterval)
    {}
     CHIP_ERROR DoSubscribe()
    {
        return SubscribeAttribute(mDevice, mEndpointId, mClusterId, mAttributeId, mMinInterval, mMaxInterval,
                                  chip::Optional<bool>(true), chip::NullOptional, chip::NullOptional);
    }
private:
    // TODO: Create all required fields to create and manage a subscription
    chip::DeviceProxy * mDevice;
    std::vector<chip::EndpointId> mEndpointId;
    std::vector<chip::ClusterId> mClusterId;
    std::vector<chip::AttributeId> mAttributeId;
    uint16_t mMinInterval;
    uint16_t mMaxInterval;

   
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