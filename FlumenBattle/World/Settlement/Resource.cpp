#include "Resource.h"
#include "FlumenBattle/World/Settlement/Types.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/SettlementTile.h"
#include "FlumenBattle/World/Tile/WorldTile.h"
#include "FlumenBattle/World/Settlement/Building.h"
#include "FlumenBattle/World/Settlement/Job.h"
#include "FlumenBattle/World/Settlement/Cohort.h"
#include "FlumenBattle/World/WorldScene.h"

using namespace world::settlement;

struct Food : public ResourceType
{
    Food() : ResourceType(ResourceTypes::FOOD, "Food", "Radish", 10) 
    {
        PopulationConsumption = 1; 
        IsProductionTileBased = true;
    }
};

struct Timber : public ResourceType
{
    Timber() : ResourceType(ResourceTypes::TIMBER, "Timber", "Timber", 10) 
    {
        IsProductionTileBased = true;
    }
};

struct Metal : public ResourceType
{
    Metal() : ResourceType(ResourceTypes::METAL, "Metal", "Metal", 100) 
    {
        IsProductionTileBased = true; 
        InputResources = {{ResourceTypes::TIMBER, 10}};
        OutputAmount = 1;
    }
};

struct Lumber : public ResourceType
{
    Lumber() : ResourceType(ResourceTypes::LUMBER, "Lumber", "Plank", 25) 
    {
        IsProductionTileBased = false; 
        RelatedModifiers = {Modifiers::WOOD_RELATED_RESOURCE_PRODUCTION}; 
        InputResources = {{ResourceTypes::TIMBER, 3}};
        OutputAmount = 2;
    }
};

struct Fiber : public ResourceType
{
    Fiber() : ResourceType(ResourceTypes::FIBER, "Fiber", "Wool", 10) 
    {
        IsProductionTileBased = true;
    }
};

struct Furniture : public ResourceType
{
    Furniture() : ResourceType(ResourceTypes::FURNITURE, "Furniture", "Furniture_WildStyle32", 70) 
    {
        IsProductionTileBased = false; 
        RelatedModifiers = {Modifiers::WOOD_RELATED_RESOURCE_PRODUCTION};
        InputResources = {{ResourceTypes::LUMBER, 3}};
        OutputAmount = 2;
    }
};

struct CookedFood : public ResourceType
{
    CookedFood() : ResourceType(ResourceTypes::COOKED_FOOD, "Cooked food", "Drumstick", 70) 
    {
        PopulationConsumption = 1; 
        IsProductionTileBased = false;
        InputResources = {{ResourceTypes::FOOD, 3}, {ResourceTypes::TIMBER, 1}};
        OutputAmount = 2;
    }
};

struct Fabric : public ResourceType
{
    Fabric() : ResourceType(ResourceTypes::FABRIC, "Fabric", "Fabric", 40) 
    {
        IsProductionTileBased = false; 
        InputResources = {{ResourceTypes::FIBER, 3}};
        OutputAmount = 3;
    }
};

struct Clothing : public ResourceType
{
    Clothing() : ResourceType(ResourceTypes::CLOTHING, "Clothing", "Clothing", 70) 
    {
        PopulationConsumption = 2; 
        IsProductionTileBased = false; 
        InputResources = {{ResourceTypes::FABRIC, 3}};
        OutputAmount = 2;
    }
};

struct Clay : public ResourceType
{
    Clay() : ResourceType(ResourceTypes::CLAY, "Clay", "Clay", 10) 
    {
        IsProductionTileBased = true;
    }
};

struct Pottery : public ResourceType
{
    Pottery() : ResourceType(ResourceTypes::POTTERY, "Pottery", "Pottery", 50) 
    {
        PopulationConsumption = 2; 
        IsProductionTileBased = false; 
        InputResources = {{ResourceTypes::TIMBER, 4}, {ResourceTypes::CLAY, 3}};
        OutputAmount = 2;
    }
};

int Resource::GetPotentialProduction(const Settlement &settlement) const
{
    return GetProductionFromBuildings(settlement) + GetProductionFromTiles(settlement);
}

int Resource::GetPotentialMidtermOutput(const ResourceHandler &handler) const
{
    static const auto &time = WorldScene::Get()->GetTime();
    static const auto ticks = time.GetTicksFromDays(3);

    if(handler.GetParent()->IsDeepSettlement() == true)
    {
        auto cycleCount = ticks / ResourceHandler::CYCLE_LENGTH;

        auto outputPerCycle = this->Workforce * Type->OutputAmount;

        return cycleCount * outputPerCycle;

        /*auto jobCount = 0;
        for(auto &job : handler.GetJobs())
        {
            if(job.GetResource() == this->Type->Type)
            {

            }
        }*/
    }
    else
    {
        auto cycleCount = ticks / ResourceHandler::CYCLE_LENGTH;

        auto outputPerCycle = this->Workforce * Type->OutputAmount;

        return cycleCount * outputPerCycle;
    }
}

int Resource::GetPotentialMidtermInput(const ResourceHandler &handler) const
{
    static const auto &time = WorldScene::Get()->GetTime();
    static const auto ticks = time.GetTicksFromDays(3);

    if(handler.GetParent()->IsDeepSettlement() == true)
    {
        auto cycleCount = ticks / ResourceHandler::CYCLE_LENGTH;

        auto inputAmount = 0;

        for(auto &resource : handler.GetResources())
        {
            if(resource.Workforce == 0)
                continue;

            for(auto &input : resource.Type->InputResources)
            {
                if(input.Resource == this->Type->Type)
                    inputAmount += input.Amount * cycleCount * resource.Workforce;
            }
        }

        return inputAmount;

        /*auto jobCount = 0;
        for(auto &job : handler.GetJobs())
        {
            if(job.GetResource() == this->Type->Type)
            {

            }
        }*/
    }
    else
    {
        auto cycleCount = ticks / ResourceHandler::CYCLE_LENGTH;

        auto outputPerCycle = this->Workforce * Type->OutputAmount;

        return cycleCount * outputPerCycle;
    }
}

int Resource::GetProductionFromBuildings(const Settlement &settlement) const
{
    if(Type->IsProductionTileBased == true)
        return 0;

    auto &buildings = settlement.GetBuildingsThatProduce(Type->Type);

    bool canProduce = true;
    for(auto &building : buildings)
    {
        for(auto &inputResource : building->GetInputResources())
        {
            auto resource = settlement.GetResource(inputResource.Resource);
            if(resource->CanFulfillOrders == false)
            {
                canProduce = false;
            }
        }
    }

    if(Storage >= settlement.GetStorage())
        canProduce = false;

    auto production = 0;
    /*if(canProduce == false)
    {
        for(auto &building : buildings)
        {
            for(auto &inputResource : building->GetInputResources())
            {
                auto resource = settlement.GetResource(inputResource.Resource);
            
                resource->Order -= inputResource.Amount * building->GetPersonnelCount();
            }
        }
    }
    else*/
    if(canProduce == true)
    {
        for(auto &building : buildings)
        {
            production += building->GetOutputResource().Amount * building->GetPersonnelCount();

            for(auto modifier : Type->RelatedModifiers) 
            {
                production += settlement.GetModifier(modifier);
            }
        }
    }

    return production;
}

int Resource::GetProductionFromTiles(const Settlement &settlement) const
{
    if(Type->IsProductionTileBased == false)
        return 0;

    if(Storage >= settlement.storage)
        return 0;

    auto production = 0;
    for(auto &tile : settlement.tiles)
    {
        if(Type->Type != ResourceTypes::METAL)
            if(tile.IsWorked == false)
                continue;

        production += tile.Tile->GetResource(Type->Type);

        production += tile.GetImprovementBonus(Type->Type);

        /*if(Type->DoesImprovementWorkAnywhere == true)
        {
            if(tile.IsBuilt)
            {
                production++;
            }
        }
        else
        {
            if(tile.Tile->GetResource(Type->Type) > 0 && tile.IsBuilt)
            {
                production++;
            }
        }*/

        /*if(tile.Tile->Biome->Type == world::WorldBiomes::DESERT)
        {
            production += GetModifier(Modifiers::FOOD_PRODUCTION_ON_DESERT_TILES);
        }*/
    }

    return production;
}

void Resource::PlaceOrders(const Settlement &settlement)
{
    auto consumption = 0;

    for(const auto &building : settlement.GetBuildings())
    {
        consumption += building.GetResourceConsumption(Type->Type) * building.GetPersonnelCount();    
    }

    Order = consumption;
}


void Resource::CheckOrderFullfilment()
{
    CanFulfillOrders = Storage >= Order;
}

void Resource::ExecuteOrders(const Settlement &settlement)
{
    Production = GetProductionFromTiles(settlement) + GetProductionFromBuildings(settlement);
}

void Resource::UpdateAbundance(Settlement &settlement)
{
    auto availableAmount = Production;

    auto consumption = Order;

    LongTermAbundance = AbundanceLevels();
    if(availableAmount >= consumption * 3)
    {
        LongTermAbundance = AbundanceLevels::CORNUCOPIA;
    }
    else if(availableAmount >= consumption * 2)
    {
        LongTermAbundance = AbundanceLevels::ABUNDANT;
    }
    else if(availableAmount > consumption)
    {
        LongTermAbundance = AbundanceLevels::ENOUGH;
    }
    else if(availableAmount == consumption)
    {
        LongTermAbundance = AbundanceLevels::BARELY_AVAILABLE;
    }
    else if(availableAmount * 2 > consumption)
    {
        LongTermAbundance = AbundanceLevels::LACKING;
    }
    else
    {
        LongTermAbundance = AbundanceLevels::SORELY_LACKING;
    }

    availableAmount += Storage;

    ShortTermAbundance = AbundanceLevels();
    if(availableAmount >= consumption * 3)
    {
        ShortTermAbundance = AbundanceLevels::CORNUCOPIA;
    }
    else if(availableAmount >= consumption * 2)
    {
        ShortTermAbundance = AbundanceLevels::ABUNDANT;
    }
    else if(availableAmount > consumption)
    {
        ShortTermAbundance = AbundanceLevels::ENOUGH;
    }
    else if(availableAmount == consumption)
    {
        ShortTermAbundance = AbundanceLevels::BARELY_AVAILABLE;
    }
    else if(availableAmount * 2 > consumption)
    {
        ShortTermAbundance = AbundanceLevels::LACKING;
    }
    else
    {
        ShortTermAbundance = AbundanceLevels::SORELY_LACKING;
    }

    if(Storage <= 3 * settlement.GetStorage() / 5)
    {
        AbundanceDegree = 0;
    }
    else if(Storage <= 4 * settlement.GetStorage() / 5)
    {
        AbundanceDegree = 1;
    }
    else if(Storage <= 9 * settlement.GetStorage() / 10)
    {
        AbundanceDegree = 2;
    }
    else
    {
        AbundanceDegree = 3;
    }

    if(Storage <= settlement.GetStorage() / 12)
    {
        ScarcityDegree = 3;
    }
    else if(Storage <= settlement.GetStorage() / 5)
    {
        ScarcityDegree = 2;
    }
    else if(Storage <= 2 * settlement.GetStorage() / 5)
    {
        ScarcityDegree = 1;
    }
    else
    {
        ScarcityDegree = 0;
    }
}

void Resource::UpdateStorage(Settlement &settlement)
{
    if(CanFulfillOrders == false)
        return;
        
    Storage -= Order;

    /*if(Storage < 0)
    {
        Storage = 0;
    }*/
}

void Resource::FinishProduction()
{
    Storage += Production;
}

void ResourceHandler::Initialize(const Settlement *parent)
{
    static const auto food = Food();
    *resources.Add() = {&food};

    static const auto timber = Timber();
    *resources.Add() = {&timber};

    static const auto fiber = Fiber();
    *resources.Add() = {&fiber};

    static const auto clay = Clay();
    *resources.Add() = {&clay};

    static const auto metal = Metal();
    *resources.Add() = {&metal};

    static const auto lumber = Lumber();
    *resources.Add() = {&lumber};

    static const auto furniture = Furniture();
    *resources.Add() = {&furniture};

    static const auto cookedFood = CookedFood();
    *resources.Add() = {&cookedFood};

    static const auto fabric = Fabric();
    *resources.Add() = {&fabric};

    static const auto clothing = Clothing();
    *resources.Add() = {&clothing};

    static const auto pottery = Pottery();
    *resources.Add() = {&pottery};

    this->parent = parent;

    this->workforce = 0;
}

void ResourceHandler::ResetOrders()
{
    for(auto &resource : resources)
    {
        resource.ResetOrder();
    }
}

void ResourceHandler::PlaceOrders(Settlement &settlement)
{
    if(settlement.IsDeepSettlement() == false)
    {
        for(auto &resource : resources)
        {
            resource.PlaceOrders(settlement);
        }
    }
    else
    {
        for(auto &job : jobSet.GetJobs())
        {
            job.PlaceOrders(*this);
        }
    }
}

void ResourceHandler::ExecuteOrders(Settlement &settlement)
{
    for(auto &resource : resources)
    {
        resource.CheckOrderFullfilment();
    }

    if(settlement.IsDeepSettlement() == true)
    {
        for(auto &job : jobSet.GetJobs())
        {
            job.ExecuteOrders(*this);
        }
    }
    else
    {
        for(auto &resource : resources)
        {
            resource.ExecuteOrders(settlement);
        }
    }
}

void ResourceHandler::Update(Settlement &settlement)
{
    ResetOrders();

    PlaceOrders(settlement);

    auto &popHandler = settlement.GetPopulationHandler();

    popHandler.PlaceOrders(settlement);

    ExecuteOrders(settlement);

    for(auto &resource : resources)
    {
        resource.UpdateAbundance(settlement);
    }

    if(settlement.IsDeepSettlement() == true)
    {
        for(auto &job : jobSet.GetJobs())
        {
            job.FinishProduction(*this);
        }
    }
    else
    {
        for(auto &resource : resources)
        {
            resource.FinishProduction();
        }
    }

    for(auto &resource : resources)
    {
        resource.UpdateStorage(settlement);
    }

    popHandler.UpdateNeeds(settlement);
}

void ResourceHandler::HireRandomWorker(ResourceTypes type)
{
    for(auto &cohort : parent->GetPopCohorts())
    {
        if(cohort.IsHired == true)
            continue;

        auto job = jobSet.GetJobs().Add();

        job->Initialize(&cohort, type);

        cohort.IsHired = true;

        workforce++;

        Get(type)->Workforce++;

        break;
    }
}

void ResourceHandler::FireRandomWorker(ResourceTypes type)
{
    for(auto &job : jobSet.GetJobs())
    {
        if(job.GetResource() != type)
            continue;

        job.GetCohort()->IsHired = false;

        jobSet.GetJobs().RemoveAt(&job);

        workforce--;

        Get(type)->Workforce--;

        break;
    }
}

Resource *ResourceHandler::Get(ResourceTypes type) const
{
    return resources.Find(type);
}

int ResourceHandler::GetWorkforce(ResourceTypes type) const
{
    return Get(type)->Workforce;
}

int ResourceHandler::GetPotentialMidtermOutput(ResourceTypes resource) const
{
    Get(resource)->GetPotentialMidtermOutput(*this);
}

int ResourceHandler::GetPotentialMidtermInput(ResourceTypes resource) const
{
    Get(resource)->GetPotentialMidtermInput(*this);
}

const ResourceType *ResourceFactory::CreateType(ResourceTypes type)
{
    switch(type)
    {
    case ResourceTypes::FOOD:
        static const auto food = Food();
        return &food;
    case ResourceTypes::TIMBER:
        static const auto timber = Timber();
        return &timber;
    case ResourceTypes::METAL:
        static const auto metal = Metal();
        return &metal;
    case ResourceTypes::LUMBER:
        static const auto lumber = Lumber();
        return &lumber;
    case ResourceTypes::FIBER:
        static const auto fiber = Fiber();
        return &fiber;
    case ResourceTypes::CLAY:
        static const auto clay = Clay();
        return &clay;
    case ResourceTypes::FURNITURE:
        static const auto furniture = Furniture();
        return &furniture;
    case ResourceTypes::COOKED_FOOD:
        static const auto cookedFood = CookedFood();
        return &cookedFood;
    case ResourceTypes::FABRIC:
        static const auto fabric = Fabric();
        return &fabric;
    case ResourceTypes::CLOTHING:
        static const auto clothing = Clothing();
        return &clothing;
    case ResourceTypes::POTTERY:
        static const auto pottery = Pottery();
        return &pottery;
    }
}

void ResourceAllocator::AllocateExtraData(container::PoolAllocator <Job> &allocator, ResourceHandler &handler)
{
    handler.jobSet.GetJobs().Initialize(allocator);
}

void ResourceAllocator::FreeExtraData(container::PoolAllocator <Job> &allocator, ResourceHandler &handler)
{
    handler.jobSet.GetJobs().Terminate(allocator);
}