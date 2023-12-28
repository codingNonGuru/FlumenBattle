#include "FlumenCore/Utility/Utility.hpp"
#include "FlumenCore/Container/Map.hpp"

#include "Settlement.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/WorldBiome.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Polity.h"
#include "FlumenBattle/World/Settlement/Affliction.h"
#include "FlumenBattle/World/Settlement/SettlementEvent.h"
#include "FlumenBattle/World/Settlement/SettlementProduction.h"
#include "FlumenBattle/World/Settlement/Condition.h"
#include "FlumenBattle/World/Settlement/Building.h"
#include "FlumenBattle/World/Group/GroupDynamics.h"

using namespace world::settlement;

void Settlement::Initialize(Word name, Color banner, world::WorldTile *location)
{
    this->modifierManager.Initialize();

    this->buildingManager->Initialize(this);

    this->name = name;

    this->banner = banner;

    this->location = location;

    this->population = 1;

    this->growth = 0;

    this->cultureGrowth = 0;

    *this->currentProduction = SettlementProductionFactory::Get()->Create(SettlementProductionOptions::NONE);

    auto tile = tiles.Add();
    tile->Tile = location;
    tile->IsWorked = true;
    tile->IsBuilt = false;

    auto &nearbyTiles = location->GetNearbyTiles(1);
    for(auto nearbyTile = nearbyTiles.GetStart(); nearbyTile != nearbyTiles.GetEnd(); ++nearbyTile)
    {
        if(*nearbyTile == this->location)
            continue;

        tile = tiles.Add();
        tile->Tile = *nearbyTile;
        tile->IsWorked = false;
        tile->IsBuilt = false;
    }

    for(auto &tile : tiles)
    {
        tile.Tile->AssertOwnership(this);
    }
}

world::WorldTile * Settlement::FindColonySpot()
{
    auto findColonySpot = [this] (float distance) -> WorldTile*
    {
        auto &tileRing = location->GetTileRing(distance);
        for(auto tileIterator = tileRing.GetStart(); tileIterator != tileRing.GetEnd(); ++tileIterator)
        {
            auto tile = *tileIterator;
            if(tile->IsBorderingOwnedTile())
                continue;

            if(tile->Biome->Type != world::WorldBiomes::STEPPE)
                continue;

            return tile;
        }
        return nullptr;
    };

    auto colonySpot = findColonySpot(4);
    if(colonySpot == nullptr)
    {
        colonySpot = findColonySpot(5);
        if(colonySpot == nullptr)
        {
            colonySpot = findColonySpot(6);
        }
    }

    return colonySpot;
}

void Settlement::WorkNewTile()
{
    auto workedTileCount = GetWorkedTiles();
    if(workedTileCount == population)
        return;

    for(auto &tile : tiles)
    {
        if(tile.Tile == location)
            continue;

        tile.IsWorked = false;
    }

    for(auto i = 0; i < population - 1; ++i)
    {
        bool hasFoundSteppe = false;
        for(auto &tile : tiles)
        {
            if(tile.IsWorked == true)
                continue;

            if(tile.Tile->Biome->Type != world::WorldBiomes::STEPPE)
                continue;

            hasFoundSteppe = true;
            tile.IsWorked = true;
            break;
        }

        if(hasFoundSteppe == false)
        {
            bool hasFoundWoods = false;
            for(auto &tile : tiles)
            {
                if(tile.IsWorked == true)
                    continue;

                if(tile.Tile->Biome->Type != world::WorldBiomes::WOODS && tile.Tile->Biome->Type != world::WorldBiomes::SWAMP)
                    continue;

                hasFoundWoods = true;
                tile.IsWorked = true;
                break;
            }

            if(hasFoundWoods == false)
            {
                for(auto &tile : tiles)
                {
                    if(tile.IsWorked == true)
                        continue;

                    if(tile.Tile->Biome->Type != world::WorldBiomes::DESERT)
                        continue;

                    tile.IsWorked = true;
                    break;
                }    
            }
        }
    }
}

void Settlement::GrowBorders() 
{
    if(tiles.GetSize() == tiles.GetCapacity())
        return;

    auto &tileRing = location->GetTileRing(2);

    auto findNewTile = [this, tileRing] (world::WorldBiomes biomeType) -> WorldTile *
    {
        for(auto tileIterator = tileRing.GetStart(); tileIterator != tileRing.GetEnd(); ++tileIterator)
        {
            auto tile = *tileIterator;
            if(tile->IsOwned())
                continue;

            if(tile->Biome->Type != biomeType)
                continue;

            return tile;
        }    
        return nullptr;
    };

    auto newTile = findNewTile(world::WorldBiomes::STEPPE);
    if(newTile == nullptr)
    {
        newTile = findNewTile(world::WorldBiomes::WOODS);
        if(newTile == nullptr)
        {
            newTile = findNewTile(world::WorldBiomes::DESERT);
            if(newTile == nullptr)
            {
                newTile = findNewTile(world::WorldBiomes::MARINE);
            }
        }
    }

    if(newTile)
    {
        auto tile = tiles.Add();
        tile->Tile = newTile;
        tile->IsWorked = false;
        tile->IsBuilt = false;

        newTile->AssertOwnership(this);
    }
}

void Settlement::DecideProduction()
{
    if(groupDynamics->patrolStrength - groupDynamics->banditStrength < 1)
    {
        *currentProduction = SettlementProductionFactory::Get()->Create(SettlementProductionOptions::PATROL);
    }
    else if(population >= 3)
    {
        auto colonySpot = FindColonySpot();
        if(colonySpot != nullptr)
        {
            *currentProduction = SettlementProductionFactory::Get()->Create(SettlementProductionOptions::SETTLERS, {colonySpot});
        }
    }

    if(currentProduction->Is(SettlementProductionOptions::NONE))
    {
        if(buildingManager->HasBuilding(BuildingTypes::SEWAGE) == false)
        {
            *currentProduction = SettlementProductionFactory::Get()->Create(SettlementProductionOptions::SEWAGE);
        }
        else
        {
            SettlementTile * improvementTarget = nullptr;
            for(auto &tile : tiles)
            {
                if(tile.Tile->Biome->Type == world::WorldBiomes::STEPPE && tile.IsBuilt == false)
                {
                    improvementTarget = &tile;
                }
            }

            if(improvementTarget != nullptr)
            {
                *currentProduction = SettlementProductionFactory::Get()->Create(SettlementProductionOptions::FARM, {improvementTarget});
            }
        }

        if(currentProduction->Is(SettlementProductionOptions::NONE))
        {
            if(buildingManager->HasBuilding(BuildingTypes::IRRIGATION) == false)
            {
                *currentProduction = SettlementProductionFactory::Get()->Create(SettlementProductionOptions::IRRIGATION);
            }
            else
            {
                *currentProduction = SettlementProductionFactory::Get()->Create(SettlementProductionOptions::PATROL);
            }
        }
    }
}

Color Settlement::GetRulerBanner() const
{
    return polity->GetRuler()->GetBanner();
}

world::WorldTile * Settlement::GetLocation() const
{
    return this->location;
}

Pool <SettlementTile> & Settlement::GetTiles()
{
    return tiles;
}

Integer Settlement::GetFoodProduction() const
{
    auto production = 0;
    for(auto &tile : tiles)
    {
        if(tile.IsWorked == false)
            continue;

        production += tile.Tile->GetFertility();
        if(tile.IsBuilt)
        {
            production++;
        }

        if(tile.Tile->Biome->Type == world::WorldBiomes::DESERT)
        {
            production += GetModifier(Modifiers::FOOD_PRODUCTION_ON_DESERT_TILES);
        }
    }

    return production;
}

FoodSecurity Settlement::GetFoodSecurity() const
{
    auto production = GetFoodProduction();

    auto consumption = population;

    if(production >= consumption * 3)
    {
        return FoodSecurity::CORNUCOPIA;
    }
    else if(production >= consumption * 2)
    {
        return FoodSecurity::ABUNDANT;
    }
    else if(production > consumption)
    {
        return FoodSecurity::ENOUGH;
    }
    else if(production == consumption)
    {
        return FoodSecurity::BARELY_AVAILABLE;
    }
    else if(production * 2 > consumption)
    {
        return FoodSecurity::LACKING;
    }
    else
    {
        return FoodSecurity::SORELY_LACKING;
    }
}

Integer Settlement::GetIndustrialProduction() const
{
    auto production = 0;
    for(auto &tile : tiles)
    {
        if(tile.IsWorked == false)
            continue;

        production += tile.Tile->GetIndustry();
        if(tile.IsBuilt)
        {
            production++;
        }
    }

    return production;
}

Integer Settlement::GetScienceProduction() const
{
    auto production = population >= 10 ? 2 : 1;
    production += GetModifier(Modifiers::SCIENCE_PRODUCTION);

    return production;
}

Integer Settlement::GetWorkedTiles() const
{
    auto tileCount = 0;
    for(auto &tile : tiles)
    {
        tileCount += tile.IsWorked ? 1 : 0;
    }

    return tileCount;
}

int Settlement::GetModifier(Modifiers modifier) const
{
    return modifierManager.GetAmount(modifier);
}

void Settlement::SetPolity(world::Polity *polity)
{
    this->polity = polity;
}

void Settlement::StrengthenPatrol()
{
    groupDynamics->StrengthenPatrol();
}

void Settlement::AddModifier(Modifier modifier)
{
    modifierManager.AddModifier(modifier);
}

void Settlement::AddCondition(ConditionData conditionData)
{
    conditionManager->AddCondition(conditionData);
}

void Settlement::AddBuilding(BuildingTypes type)
{
    buildingManager->AddBuilding(type);
}

void Settlement::ProcessEarthquake(const disaster::Earthquake &earthquake)
{
    BuildingDamager::DamageBuildings(earthquake, *buildingManager);

    BuildingDamager::DamageImprovements(earthquake, *this);
}

void Settlement::Update()
{
    auto &worldTime = world::WorldScene::Get()->GetTime();

    auto updateModifiers = [this]
    {
        modifierManager.ClearModifiers();

        polity->ApplyTechnologyModifiers(this);

        conditionManager->ApplyModifiers(*this);

        buildingManager->ApplyModifiers(*this);
    };

    updateModifiers();

    auto foodSecurity = GetFoodSecurity();

    switch(foodSecurity)
    {
    case FoodSecurity::CORNUCOPIA:
        growth += 3;
        break;
    case FoodSecurity::ABUNDANT:
        growth += 2;
        break;
    case FoodSecurity::ENOUGH:
        growth += 1;
        break;
    case FoodSecurity::BARELY_AVAILABLE:
        break;
    case FoodSecurity::LACKING:
        growth -= 1;
        break;
    case FoodSecurity::SORELY_LACKING:
        growth -= 2;
        break;
    }

    if(foodSecurity == FoodSecurity::LACKING || foodSecurity == FoodSecurity::SORELY_LACKING)
    {
        if(afflictions.Find(AfflictionTypes::HUNGER) == nullptr)
        {
            *afflictions.Add() = AfflictionFactory::Get()->Create(AfflictionTypes::HUNGER);
        }
    }

    if(growth >= 100)
    {
        growth = 0;
        population++;
    }
    else if(growth < 0)
    {
        growth = 0;
    }

    cultureGrowth++;

    if(cultureGrowth >= 100)
    {
        cultureGrowth = 0;

        GrowBorders();
    }

    if(worldTime.MinuteCount == 0)
    {
        auto difficultyBonus = GetModifier(Modifiers::MALARIA_EMERGENCE_DIFFICULTY);
        if(utility::GetRandom(1, 100) > 98 + difficultyBonus)
        {
            if(afflictions.Find(AfflictionTypes::MALARIA) == nullptr)
            {
                *afflictions.Add() = AfflictionFactory::Get()->Create(AfflictionTypes::MALARIA);
            }
        }

        for(auto &affliction : afflictions)
        {
            auto result = affliction.Type->OnPerform(*this, affliction);
            SettlementEventGenerator::GenerateEvent(*this, result);
        }
    }

    WorkNewTile();

    currentProduction->AddProgress(GetIndustrialProduction());
    
    if(currentProduction->IsDone())
    {
        currentProduction->Finish(*this);

        *currentProduction = SettlementProductionFactory::Get()->Create(SettlementProductionOptions::NONE);

        DecideProduction();
    }

    if(worldTime.MinuteCount == 0)
    {
        groupDynamics->Update(*this);
    }
}