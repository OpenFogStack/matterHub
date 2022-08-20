#pragma once
#include "InteractionModelHelper.h"
#include "WriteInteraction.h"
#include "controller/WriteInteraction.h"

namespace chip {
template <typename T>
static void onConnectedCallbackWrite(void * context, chip::OperationalDeviceProxy * peer_device)
{
    WriteCommandData<T> * data = reinterpret_cast<chip::WriteCommandData<T> *>(context);
    ESP_LOGI("Interaction Model Helper", "Requested Write!");

    WriteAttribute<T> * attr =
        chip::Platform::New<WriteAttribute<T>>(peer_device, data->endpointId, data->clusterId, data->attributeId, data->value);
    attr->DoWrite();
}
template <typename T>
WriteCommandData<T>::WriteCommandData(T & value) : BaseCommandData(onConnectedCallbackWrite<T>, (void *) this), value(value)
{}
} // namespace chip