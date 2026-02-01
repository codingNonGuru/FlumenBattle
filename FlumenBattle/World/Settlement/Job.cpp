#include "Job.h"
#include "Resource.h"
#include "Settlement.h"

using namespace world::settlement;

void Job::Initialize(Cohort *cohort, ResourceTypes resource)
{
    status = JobStatus::OBTAINING_MATERIALS;

    progress = 0;

    this->cohort = cohort;

    this->resourceType = resource;
}

void Job::PlaceOrders(ResourceHandler &handler)
{
    if(status != JobStatus::OBTAINING_MATERIALS)
        return;

    auto resource = handler.Get(resourceType);

    for(auto &input : resource->Type->InputResources)
    {
        handler.Get(input.Resource)->Order += input.Amount;
    }
}

void Job::ExecuteOrders(ResourceHandler &handler)
{
    if(status == JobStatus::OBTAINING_MATERIALS)
    {
        auto resource = handler.Get(resourceType);

        bool canProcureMaterials = true;
        for(auto &input : resource->Type->InputResources)
        {
            if(handler.Get(input.Resource)->CanFulfillOrders == false)
            {
                canProcureMaterials = false;
                break;
            }
        }

        if(canProcureMaterials == true)
        {
            status = JobStatus::PRODUCING;
        }
    }
    else if(status == JobStatus::PRODUCING)
    {
        progress++;

        if(progress == ResourceHandler::CYCLE_LENGTH)
        {
            status = JobStatus::DELIVERING_GOODS;
        }
    }
}

void Job::FinishProduction(ResourceHandler &handler)
{
    if(status != JobStatus::DELIVERING_GOODS)
        return;

    auto resource = handler.Get(resourceType);

    auto output = resource->Type->OutputAmount;

    if(resource->Storage + output > handler.GetParent()->GetStorage())
        return;

    resource->Storage += output;

    status = JobStatus::OBTAINING_MATERIALS;

    progress = 0;
}