#include <InteractionModel.h>

template <typename T>
class WriteAttribute : public InteractionModelWriter, public chip::app::WriteClient::Callback //, public ModelCommand
{
public:
    WriteAttribute(chip::DeviceProxy * device, chip::EndpointId endpointId, chip::ClusterId clusterId,
                   chip::AttributeId attributeId, T & value) :
        InteractionModelWriter(this),
        mDevice(device), mValue(value)
    {
        mEndpointId  = endpointId;
        mClusterId   = clusterId;
        mAttributeId = attributeId;
    }
    void DoWrite()
    {
        InteractionModelWriter::WriteAttribute(mDevice, mEndpointId, mClusterId, mAttributeId, mValue, mTimedInteractionTimeoutMs,
                                               mSuppressResponse);
    }
    chip::DeviceProxy * mDevice;
    chip::FabricId mfabricId;
    chip::EndpointId mEndpointId;
    chip::ClusterId mClusterId;
    chip::AttributeId mAttributeId;
    T & mValue;
    const chip::Optional<uint16_t> & mTimedInteractionTimeoutMs = chip::NullOptional;
    const chip::Optional<bool> & mSuppressResponse              = chip::NullOptional;
    const chip::Optional<chip::DataVersion> & mDataVersion      = chip::NullOptional;
    void OnDone(chip::app::WriteClient * apWriteClient) override { chip::Platform::Delete(this); }
};