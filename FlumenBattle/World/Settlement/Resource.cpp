#include "Resource.h"
#include "FlumenBattle/World/Settlement/Types.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/WorldTile.h"

using namespace world::settlement;

struct Food : public ResourceType
{
    Food() : ResourceType(ResourceTypes::FOOD, 10, true) {PopulationConsumption = 1;}
};

struct Timber : public ResourceType
{
    Timber() : ResourceType(ResourceTypes::TIMBER, 10) {}
};

struct Metal : public ResourceType
{
    Metal() : ResourceType(ResourceTypes::METAL, 10) {}
};

int Resource::GetProduction(const Settlement &settlement) const
{
    auto production = 0;
    for(auto &tile : settlement.tiles)
    {
        if(tile.IsWorked == false)
            continue;

        production += tile.Tile->GetResource(Type->Type);
        /*if(Type->Type == ResourceTypes::METAL)
        {
            std::cout<<tile.Tile->GetResource(Type->Type)<<"\n";
        }*/

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

void Resource::Update(Settlement &settlement)
{
    Production = GetProduction(settlement);

    auto consumption = settlement.population * Type->PopulationConsumption;

    int availableAmount = Production + Storage;

    Abundance = AbundanceLevels();
    if(availableAmount >= consumption * 3)
    {
        Abundance = AbundanceLevels::CORNUCOPIA;
    }
    else if(availableAmount >= consumption * 2)
    {
        Abundance = AbundanceLevels::ABUNDANT;
    }
    else if(availableAmount > consumption)
    {
        Abundance = AbundanceLevels::ENOUGH;
    }
    else if(availableAmount == consumption)
    {
        Abundance = AbundanceLevels::BARELY_AVAILABLE;
    }
    else if(availableAmount * 2 > consumption)
    {
        Abundance = AbundanceLevels::LACKING;
    }
    else
    {
        Abundance = AbundanceLevels::SORELY_LACKING;
    }

    Storage += Production - consumption;

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
}

void ResourceHandler::Update(Settlement &settlement)
{
    for(auto &resource : resources)
    {
        resource.Update(settlement);
    }
}

Resource *ResourceHandler::Get(ResourceTypes type) const
{
    return resources.Find(type);
}