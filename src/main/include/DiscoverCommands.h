#include "BaseCommand.h"

namespace shell{
void DescribeWorkerFunction(intptr_t context);
void RegisterDiscoverCommands();
struct DiscoverCommandData:BaseCommandData{
    DiscoverCommandData();
};
}