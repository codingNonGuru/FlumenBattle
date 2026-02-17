#include "Job.h"
#include "Resource.h"
#include "Settlement.h"
#include "SettlementTile.h"
#include "Cohort.h"
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

        if(progress == ResourceHandler::PRODUCTION_DURATION)
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

        auto output = GetOutput(doesHappenInBuilding);

        if(resource->Storage + output > handler.GetParent()->GetStorage())
            return;

        resource->Storage += output;

        status = JobStatus::OBTAINING_MATERIALS;

        progress = 0;
    }
    else
    {
        static const auto resources = {ResourceTypes::FOOD, ResourceTypes::CLAY, ResourceTypes::TIMBER, ResourceTypes::FIBER, ResourceTypes::ORE};

        for(auto resourceType : resources)
        {
            auto output = GetOutput(resourceType);

            auto resource = handler.Get(resourceType);

            if(resource->Storage + output > handler.GetParent()->GetStorage())
                return;

            resource->Storage += output;
        }

        status = JobStatus::OBTAINING_MATERIALS;

        progress = 0;
    }
}

int Job::GetOutput(bool isHappeningInBuilding) const
{
    if(tile != nullptr)
        return 0;
    
    auto output = ResourceFactory::Get()->CreateType(resourceType)->OutputAmount;

    if(isHappeningInBuilding == true)
        output += Resource::PRODUCTION_BOOST_PER_BUILDING;

    if(cohort->Race->HasAffinityFor(resourceType) == true)
        output += Resource::PRODUCTION_BOOST_FROM_RACE;

    if(cohort->GetLevel() == 5)
    {
        output += Resource::PRODUCTION_BOOST_FROM_EXPERIENCE * 2;
    }
    else if(cohort->GetLevel() > 1)
    {
        output += Resource::PRODUCTION_BOOST_FROM_EXPERIENCE;
    }

    return output;
}

int Job::GetOutput(ResourceTypes type) const
{
    if(tile == nullptr)
        return 0;

    auto output = tile->Tile->GetResource(type);

    if(cohort->Race->HasAffinityFor(type) == true)
        output += Resource::PRODUCTION_BOOST_FROM_RACE;

    if(cohort->GetLevel() == 5)
    {
        output += Resource::PRODUCTION_BOOST_FROM_EXPERIENCE * 2;
    }
    else if(cohort->GetLevel() > 1)
    {
        output += Resource::PRODUCTION_BOOST_FROM_EXPERIENCE;
    }

    return output;
}