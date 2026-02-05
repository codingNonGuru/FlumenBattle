#include "Job.h"
#include "Resource.h"
#include "Settlement.h"
#include "SettlementTile.h"
#include "FlumenBattle/World/Tile/WorldTile.h"

using namespace world::settlement;

void Job::Initialize(Cohort *cohort, ResourceTypes resource)
{
    status = JobStatus::OBTAINING_MATERIALS;

    progress = 0;

    this->cohort = cohort;

    this->resourceType = resource;

    this->tile = nullptr;
}

void Job::Initialize(Cohort *cohort, SettlementTile *tile)
{
    status = JobStatus::OBTAINING_MATERIALS;

    progress = 0;

    this->cohort = cohort;

    this->resourceType = ResourceTypes::NONE;

    this->tile = tile;
}

void Job::PlaceOrders(ResourceHandler &handler)
{
    if(status != JobStatus::OBTAINING_MATERIALS)
        return;

    if(this->tile == nullptr)
    {
        auto resource = handler.Get(resourceType);

        for(auto &input : resource->Type->InputResources)
        {
            handler.Get(input.Resource)->Order += input.Amount;
        }
    }
    else
    {

    }
}

void Job::ExecuteOrders(ResourceHandler &handler)
{
    if(status == JobStatus::OBTAINING_MATERIALS)
    {
        if(this->tile == nullptr)
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
        else
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

void Job::FinishProduction(ResourceHandler &handler, bool doesHappenInBuilding)
{
    if(status != JobStatus::DELIVERING_GOODS)
        return;

    if(this->tile == nullptr)
    {
        auto resource = handler.Get(resourceType);

        auto output = resource->Type->OutputAmount;
        if(doesHappenInBuilding == true)
        {
            output += Resource::PRODUCTION_BOOST_PER_BUILDING;
        }

        if(resource->Storage + output > handler.GetParent()->GetStorage())
            return;

        resource->Storage += output;

        status = JobStatus::OBTAINING_MATERIALS;

        progress = 0;
    }
    else
    {
        static const auto resources = {ResourceTypes::FOOD, ResourceTypes::CLAY, ResourceTypes::TIMBER, ResourceTypes::FIBER, ResourceTypes::METAL};

        for(auto resourceType : resources)
        {
            auto output = this->tile->Tile->GetResource(resourceType);

            auto resource = handler.Get(resourceType);

            if(resource->Storage + output > handler.GetParent()->GetStorage())
            return;

            resource->Storage += output;
        }

        status = JobStatus::OBTAINING_MATERIALS;

        progress = 0;
    }
}