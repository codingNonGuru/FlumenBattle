#include "Resource.h"
#include "FlumenBattle/World/Settlement/Types.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/Settlement/Building.h"

using namespace world::settlement;

struct Food : public ResourceType
{
    Food() : ResourceType(ResourceTypes::FOOD, 10, true) {PopulationConsumption = 1; IsProductionTileBased = true;}
};

struct Timber : public ResourceType
{
    Timber() : ResourceType(ResourceTypes::TIMBER, 10) {IsProductionTileBased = true;}
};

struct Metal : public ResourceType
{
    Metal() : ResourceType(ResourceTypes::METAL, 10) {IsProductionTileBased = true;}
};

struct Lumber : public ResourceType
{
    Lumber() : ResourceType(ResourceTypes::LUMBER, 25) {IsProductionTileBased = false;}
};

int Resource::GetProductionFromBuildings(const Settlement &settlement) const
{
    if(Type->IsProductionTileBased == true)
        return 0;

    auto &buildings = settlement.GetBuildingsThatProduce(Type->Type);

    bool canProduce = true;
    for(auto &building : buildings)
    {
        auto inputResource = building->GetInputResource();
        auto resource = settlement.GetResource(inputResource.Resource);
        if(resource->Order > resource->Storage)
        {
            canProduce = false;
        }
    }

    if(canProduce == false)
        return 0;

    auto production = 0;
    for(auto &building : buildings)
    {
        production += building->GetOutputResource().Amount;
    }

    return production;
}

int Resource::GetProductionFromTiles(const Settlement &settlement) const
{
    if(Type->IsProductionTileBased == false)
        return 0;

    auto production = 0;
    for(auto &tile : settlement.tiles)
    {
        if(tile.IsWorked == false)
            continue;

        production += tile.Tile->GetResource(Type->Type);

        if(Type->DoesImprovementWorkAnywhere == true)
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
        }

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
        consumption += building.GetResourceConsumption(Type->Type);    
    }

    consumption += settlement.GetPopulation() * Type->PopulationConsumption;

    Order = consumption;
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
}

void Resource::UpdateStorage(Settlement &settlement)
{
    Storage += Production;

    Storage -= Order;

    if(Storage > settlement.storage)
    {
        Storage = settlement.storage;
    }
    else if(Storage < 0)
    {
        Storage = 0;
    }
}

void ResourceHandler::Initialize()
{
    static const auto food = Food();
    *resources.Add() = {&food};

    static const auto timber = Timber();
    *resources.Add() = {&timber};

    static const auto metal = Metal();
    *resources.Add() = {&metal};

    static const auto lumber = Lumber();
    *resources.Add() = {&lumber};
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
    for(auto &resource : resources)
    {
        resource.PlaceOrders(settlement);
    }
}

void ResourceHandler::ExecuteOrders(Settlement &settlement)
{
    for(auto &resource : resources)
    {
        resource.ExecuteOrders(settlement);
    }
}

void ResourceHandler::Update(Settlement &settlement)
{
    ResetOrders();

    PlaceOrders(settlement);

    ExecuteOrders(settlement);

    for(auto &resource : resources)
    {
        resource.UpdateAbundance(settlement);
    }

    for(auto &resource : resources)
    {
        resource.UpdateStorage(settlement);
    }
}

Resource *ResourceHandler::Get(ResourceTypes type) const
{
    return resources.Find(type);
}