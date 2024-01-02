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
#include "FlumenBattle/World/Settlement/Path.h"

using namespace world::settlement;

#define TIME_BETWEEN_SHIPMENTS 12

#define VOLUME_PER_SHIPMENT 10

#define STORAGE_THRESHOLD 100

#define SHIPMENT_VOLUME_LOSS 3

bool Link::operator== (const settlement::Path &path) const 
{
    return *Path == path;
}

void Settlement::Initialize(Word name, Color banner, world::WorldTile *location)
{
    this->modifierManager.Initialize();

    this->buildingManager->Initialize(this);

    this->name = name;

    this->banner = banner;

    this->location = location;

    this->population = 1;

    this->growth = 0;

    this->foodStorage = 0;

    this->metalStorage = 0;

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

void Settlement::AddPath(Path *path) 
{
    *links.Add() = {path};
}

Path *Settlement::GetPathTo(Settlement *settlement) 
{
    for(auto &link : links)
    {
        if(*link.Path == Path(this, settlement))
            return link.Path;
    }

    return nullptr;
}

static Array <world::WorldTile *> candidateTiles = Array <world::WorldTile *> (128);

world::WorldTile * Settlement::FindColonySpot()
{
    candidateTiles.Reset();

    auto bestChance = INT_MAX;
    WorldTile *bestTile = nullptr;
    for(int i = MINIMUM_COLONIZATION_RANGE; i <= MAXIMUM_COLONIZATION_RANGE; ++i)
    {
        auto &tileRing = location->GetTileRing(i);
        for(auto tileIterator = tileRing.GetStart(); tileIterator != tileRing.GetEnd(); ++tileIterator)
        {
            auto tile = *tileIterator;
            if(tile->IsBorderingOwnedTile())
                continue;

            if(tile->Biome->Type != world::WorldBiomes::STEPPE)
                continue;

            auto &interestMap = this->GetPolity()->GetInterestMap();
            auto prospectedTile = interestMap.GetTile(tile->HexCoordinates);
            auto stakeholder = prospectedTile->Owner;
            if(stakeholder != nullptr && stakeholder != this)
                continue;

            if(prospectedTile->Distance < bestChance)
            {
                bestChance = prospectedTile->Distance;
                bestTile = tile;
            }
        }
    }

    if(bestTile == nullptr)
    {
        return nullptr;
    }

    for(int i = MINIMUM_COLONIZATION_RANGE; i <= MAXIMUM_COLONIZATION_RANGE; ++i)
    {
        auto &tileRing = location->GetTileRing(i);
        for(auto tileIterator = tileRing.GetStart(); tileIterator != tileRing.GetEnd(); ++tileIterator)
        {
            auto tile = *tileIterator;
            if(tile->IsBorderingOwnedTile())
                continue;

            if(tile->Biome->Type != world::WorldBiomes::STEPPE)
                continue;

            auto &interestMap = this->GetPolity()->GetInterestMap();
            auto prospectedTile = interestMap.GetTile(tile->HexCoordinates);
            auto stakeholder = prospectedTile->Owner;
            if(stakeholder != nullptr && stakeholder != this)
                continue;

            if(prospectedTile->Distance == bestChance)
            {
                *candidateTiles.Add() = tile;
            }
        }
    }

    auto candidate = candidateTiles.GetRandom();
    if(candidate == nullptr)
    {
        return nullptr;
    }
    else
    {
        return *candidate;
    }
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

Integer Settlement::GetMetalProduction() const
{
    auto production = 0;
    for(auto &tile : tiles)
    {
        if(tile.IsWorked == false)
            continue;

        production += tile.Tile->GetMetal();
        if(tile.IsBuilt)
        {
            production++;
        }

        /*if(tile.Tile->Biome->Type == world::WorldBiomes::DESERT)
        {
            production += GetModifier(Modifiers::FOOD_PRODUCTION_ON_DESERT_TILES);
        }*/
    }

    return production;
}

void Settlement::UpdateFoodSituation()
{
    foodProduction = GetFoodProduction();

    auto consumption = population;

    int availableFood = foodProduction + foodStorage;

    foodSecurity = FoodSecurity();
    if(availableFood >= consumption * 3)
    {
        foodSecurity = FoodSecurity::CORNUCOPIA;
    }
    else if(availableFood >= consumption * 2)
    {
        foodSecurity = FoodSecurity::ABUNDANT;
    }
    else if(availableFood > consumption)
    {
        foodSecurity = FoodSecurity::ENOUGH;
    }
    else if(availableFood == consumption)
    {
        foodSecurity = FoodSecurity::BARELY_AVAILABLE;
    }
    else if(availableFood * 2 > consumption)
    {
        foodSecurity = FoodSecurity::LACKING;
    }
    else
    {
        foodSecurity = FoodSecurity::SORELY_LACKING;
    }

    foodStorage += foodProduction - consumption;

    if(foodStorage > 200)
    {
        foodStorage = 200;
    }
    else if(foodStorage < 0)
    {
        foodStorage = 0;
    }
}

void Settlement::UpdateMetalSituation()
{
    auto production = polity->GetRuler() == this ? 1 : 0; //GetMetalProduction();

    //auto consumption = population;

    //int availableFood = foodProduction + foodStorage;

    metalStorage += production;// - consumption;

    if(metalStorage > 200)
    {
        metalStorage = 200;
    }
    else if(metalStorage < 0)
    {
        metalStorage = 0;
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

    return 10;//production;
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

    UpdateFoodSituation();

    UpdateMetalSituation();

    switch(foodSecurity)
    {
    case FoodSecurity::CORNUCOPIA:
        growth += 5;
        break;
    case FoodSecurity::ABUNDANT:
        growth += 4;
        break;
    case FoodSecurity::ENOUGH:
        growth += 3;
        break;
    case FoodSecurity::BARELY_AVAILABLE:
        break;
    case FoodSecurity::LACKING:
        growth -= 3;
        break;
    case FoodSecurity::SORELY_LACKING:
        growth -= 5;
        break;
    }

    if(foodSecurity == FoodSecurity::LACKING || foodSecurity == FoodSecurity::SORELY_LACKING)
    {
        if(afflictions.Find(AfflictionTypes::HUNGER) == nullptr)
        {
            *afflictions.Add() = AfflictionFactory::Get()->Create(AfflictionTypes::HUNGER);
        }
    }

    if(growth >= 200)
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
            EventGenerator::GenerateEvent(*this, result);
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

void Settlement::PrepareTransport()
{
    lastShipmentTime--;

    if(lastShipmentTime < 0)
    {
        lastShipmentTime = 0;
    }

    if(lastShipmentTime > 0)
        return;

    if(metalStorage < STORAGE_THRESHOLD)
        return;

    for(auto &link : links)
    {
        if(link.HasShipped == true)
            continue;

        auto other = link.Path->GetOther(this);

        if(other->GetMetalStorage() > STORAGE_THRESHOLD || other->GetMetalStorage() > metalStorage)
            continue;

        lastShipmentTime = TIME_BETWEEN_SHIPMENTS;
        shipment = {other};
        link.HasShipped = true;

        break;
    }

    bool hasShippedToAll = true;
    for(auto &link : links)
    {
        if(link.HasShipped == true)
            continue;

        auto other = link.Path->GetOther(this);

        bool cannotSendToOther = other->GetMetalStorage() > STORAGE_THRESHOLD || other->GetMetalStorage() > metalStorage;
        if(cannotSendToOther == true)
            continue;
        
        hasShippedToAll = false;
    }

    if(hasShippedToAll == true)
    {
        for(auto &link : links)
        {
            link.HasShipped = false;
        }   
    }
}

void Settlement::SendTransport()
{
    if(shipment.To == nullptr)
        return;

    metalStorage -= VOLUME_PER_SHIPMENT;

    shipment.To->ReceiveTransport();

    shipment.To = nullptr;
}

void Settlement::ReceiveTransport()
{
    metalStorage += VOLUME_PER_SHIPMENT - SHIPMENT_VOLUME_LOSS;
}