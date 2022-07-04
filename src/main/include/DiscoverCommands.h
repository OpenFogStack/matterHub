#include "BaseCommand.h"

#include "Subscription.h"
#include "platform/CHIPDeviceLayer.h"
#include <functional>
#include <map>


namespace shell {
void DescribeWorkerFunction(intptr_t context);
void RegisterDiscoverCommands();
struct DiscoverCommandData : BaseCommandData
{
    DiscoverCommandData();
};

void onPartListReadCallback(const chip::app::ConcreteDataAttributePath & path, chip::TLV::TLVReader * data, void * context);
void onServerListReadCallback(const chip::app::ConcreteDataAttributePath & path, chip::TLV::TLVReader * data, void * context);
void onAttributesReadCallback(const chip::app::ConcreteDataAttributePath& path, chip::TLV::TLVReader* data, void* context);
void onCommandsReadCallback(const chip::app::ConcreteDataAttributePath& path, chip::TLV::TLVReader* data, void* context);

// void onClientListReadCallback(const chip::app::ConcreteDataAttributePath & path, chip::TLV::TLVReader * data, void * context);
// void onDeviceListReadCallback(const chip::app::ConcreteDataAttributePath & path, chip::TLV::TLVReader * data, void * context);

struct Cluster{
    Cluster(chip::ClusterId id):id(id){
        attributes = {};
        commands = {};
    }

    chip::ClusterId id;
    std::vector<chip::AttributeId> attributes;
    std::vector<chip::CommandId> commands;

};

struct Endpoint{
    Endpoint(chip::EndpointId id):id(id){
        clusters = {};
    }

    chip::EndpointId id;
    std::map<chip::ClusterId,Cluster> clusters;
};

class DescriptionManager{
    private:
    Subscription * lastSub = nullptr;

    public: 
    DescriptionManager(chip::DeviceProxy * peer_device):mDevice(peer_device){
        mEndpoints = {};
        init();
    }

    ~DescriptionManager(){
        // cleanup, doesn't work here. see comment below
        // if(lastSub != nullptr)
        //     chip::Platform::Delete(lastSub);
    }

    void ReadAttribute(chip::EndpointId endpointId, chip::ClusterId clusterId,
                       chip::AttributeId attributeId, SubscriptionCallback callback)
    {
        // cleanup, doesn't work here. unsure why tho. maybe there is more background work being done by the extended chip classes.
        // if(lastSub != nullptr)
        //     chip::Platform::Delete(lastSub);
        
        lastSub = chip::Platform::New<Subscription>(mDevice, endpointId, clusterId, attributeId, callback,this);
        CHIP_ERROR error   = lastSub->Read();
        if (error == CHIP_NO_ERROR)
        {
            ESP_LOGI("Discover", "Succesfull Attribute Read Request for:");
            ESP_LOGI("Discover", "  Node: '0x%02llx' Endpoint: '0x%02x' Cluster: '0x%02x' Attribute: '0x%02x'",
                     mDevice->GetDeviceId(), endpointId, clusterId, attributeId);
        }
    }

    chip::DeviceProxy * mDevice;
    std::map<chip::EndpointId,Endpoint> mEndpoints;
    std::map<chip::EndpointId,Endpoint>::iterator currentEndpoint;
    std::map<chip::ClusterId,Cluster>::iterator currentCluster;



    private:
    void init()
    {
        using namespace std::placeholders;
        ESP_LOGI("Discover", "DiscoverManagerInit:");
        ESP_LOGI("Discover", " - NodeId: '0x%02llx'", mDevice->GetDeviceId());
        ESP_LOGI("Discover", " - Endpoint ID: '0x0000'");
        ESP_LOGI("Discover", " - Cluster ID: '0x%02x'", chip::app::Clusters::Descriptor::Id);
        ESP_LOGI("Discover", " - Attribute ID: '0x%02x'", chip::app::Clusters::Descriptor::Attributes::PartsList::Id);
        
        ReadAttribute(0, chip::app::Clusters::Descriptor::Id,
                                        chip::app::Clusters::Descriptor::Attributes::PartsList::Id, onPartListReadCallback);
    }
};
}
