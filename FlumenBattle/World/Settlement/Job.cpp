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

    this->isUsingBuilding = false;

    this->finishedCycleCount = 0;

    this->timeSpent = 0;
}

void Job::Initialize(Cohort *cohort, SettlementTile *tile)
{
    status = JobStatus::OBTAINING_MATERIALS;

    progress = 0;

    this->cohort = cohort;

    this->resourceType = ResourceTypes::NONE;

    this->tile = tile;

    this->isUsingBuilding = false;

    this->finishedCycleCount = 0;

    this->timeSpent = 0;
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

void Job::CheckMaterialAvailability(ResourceHandler &handler)
{
    if(status != JobStatus::OBTAINING_MATERIALS)
        return;

    if(this->tile != nullptr)
        return;
        
    auto resource = handler.Get(resourceType);

    canProcureMaterials = true;
    for(auto &input : resource->Type->InputResources)
    {
        if(handler.Get(input.Resource)->CanFulfillOrders == false)
        {
            canProcureMaterials = false;
            break;
        }
    }

    if(canProcureMaterials == true)
        return;

    for(auto &input : resource->Type->InputResources)
    {
        auto inputResource = handler.Get(input.Resource);
        inputResource->Order -= input.Amount;
    }
}

void Job::ExecuteOrders(ResourceHandler &handler)
{
    if(status == JobStatus::OBTAINING_MATERIALS)
    {
        if(this->tile == nullptr)
        {
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

    timeSpent++;
}

void Job::FinishProduction(ResourceHandler &handler)
{
    if(status != JobStatus::DELIVERING_GOODS)
        return;

    if(this->tile == nullptr)
    {
        auto resource = handler.Get(resourceType);

        auto output = GetOutput(handler.GetParent());

        if(resource->Storage + output > handler.GetParent()->GetStorage())
            return;

        resource->Storage += output;

        status = JobStatus::OBTAINING_MATERIALS;

        progress = 0;

        finishedCycleCount++;

        if(handler.GetToolStock() > 0)
            handler.ConsumeToolCharge();
    }
    else
    {
        for(auto resourceType : BASIC_RESOURCES)
        {
            auto output = GetOutput(resourceType, handler.GetParent());

            auto resource = handler.Get(resourceType);

            if(resource->Storage + output > handler.GetParent()->GetStorage())
                continue;

            resource->Storage += output;
        }

        status = JobStatus::OBTAINING_MATERIALS;

        progress = 0;

        finishedCycleCount++;

        if(handler.GetToolStock() > 0)
            handler.ConsumeToolCharge();
    }

    timeSpent++;
}

int Job::GetOutput(const Settlement *settlement) const
{
    if(tile != nullptr)
        return 0;
    
    auto output = ResourceFactory::Get()->CreateType(resourceType)->OutputAmount;

    if(isUsingBuilding == true)
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

    if(settlement->GetResourceHandler().HasToolProductionBonus() == true)
    {
        output += 1;
    }

    return output;
}

int Job::GetOutput(ResourceTypes type, const Settlement *settlement) const
{
    if(tile == nullptr)
        return 0;

    auto output = tile->Tile->GetResource(type);

    if(output == 0)
        return 0;

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

    if(settlement->GetResourceHandler().HasToolProductionBonus() == true)
    {
        output += 1;
    }

    return output;
}