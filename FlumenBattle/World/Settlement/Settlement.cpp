#include "FlumenCore/Utility/Utility.hpp"
#include "FlumenCore/Container/Map.hpp"

#include "Settlement.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/WorldBiome.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Polity.h"
#include "FlumenBattle/World/Faction.h"
#include "FlumenBattle/World/Settlement/Affliction.h"
#include "FlumenBattle/World/Settlement/SettlementEvent.h"
#include "FlumenBattle/World/Settlement/SettlementProduction.h"
#include "FlumenBattle/World/Settlement/Condition.h"
#include "FlumenBattle/World/Settlement/Building.h"
#include "FlumenBattle/World/Group/GroupDynamics.h"
#include "FlumenBattle/World/Settlement/Path.h"
#include "FlumenBattle/Utility/Pathfinder.h"

using namespace world::settlement;

#define TIME_BETWEEN_SHIPMENTS 12

#define VOLUME_PER_SHIPMENT 10

#define STORAGE_THRESHOLD 200

#define SHIPMENT_VOLUME_LOSS 3

bool Link::operator== (const settlement::Path &path) const 
{
    return *Path == path;
}

void Settlement::Initialize(Word name, Color banner, world::WorldTile *location)
{
    this->buildingManager->Initialize(this);

    this->resourceHandler.Initialize();

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
            auto stakeholder = prospectedTile->GetOwner();
            if(stakeholder != nullptr && stakeholder != this)
                continue;

            if(prospectedTile->GetCost() < bestChance)
            {
                bestChance = prospectedTile->GetCost();
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
            auto stakeholder = prospectedTile->GetOwner();
            if(stakeholder != nullptr && stakeholder != this)
                continue;

            if(prospectedTile->GetCost() == bestChance)
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

    if(areNearbyTilesTaken == true)
        return;

    auto findNewTileInRing = [this] (int distance) -> WorldTile *
    {
        auto &tileRing = location->GetTileRing(distance);

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
    };

    auto newTile = findNewTileInRing(2);
    if(newTile != nullptr)
    {
        auto tile = tiles.Add();
        tile->Tile = newTile;
        tile->IsWorked = false;
        tile->IsBuilt = false;

        newTile->AssertOwnership(this);
    }
    else
    {
        newTile = findNewTileInRing(3);
        if(newTile != nullptr)
        {
            auto tile = tiles.Add();
            tile->Tile = newTile;
            tile->IsWorked = false;
            tile->IsBuilt = false;

            newTile->AssertOwnership(this);
        }
        else
        {
            areNearbyTilesTaken = true;
        }
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

Integer Settlement::GetIndustrialProduction() const
{
    auto production = GetPopulation();
    /*for(auto &tile : tiles)
    {
        if(tile.IsWorked == false)
            continue;

        production += tile.Tile->GetIndustry();
        if(tile.IsBuilt)
        {
            production++;
        }
    }*/

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

Settlement *Settlement::GetRuler() const
{
    return polity->GetRuler();
}

int Settlement::GetModifier(Modifiers modifier) const
{
    return modifierManager.GetAmount(modifier);
}

void Settlement::SetPolity(polity::Polity *polity)
{
    this->polity = polity;
}

void Settlement::KillPopulation()
{
    population--;
    if(population < 0)
    {
        population = 0;
    }
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

    resourceHandler.Update(*this);

    auto foodSecurity = resourceHandler.Get(ResourceTypes::FOOD)->Abundance;
    switch(foodSecurity)
    {
    case AbundanceLevels::CORNUCOPIA:
        growth += 5;
        break;
    case AbundanceLevels::ABUNDANT:
        growth += 4;
        break;
    case AbundanceLevels::ENOUGH:
        growth += 3;
        break;
    case AbundanceLevels::BARELY_AVAILABLE:
        break;
    case AbundanceLevels::LACKING:
        growth -= 3;
        break;
    case AbundanceLevels::SORELY_LACKING:
        growth -= 5;
        break;
    }

    if(foodSecurity == AbundanceLevels::LACKING || foodSecurity == AbundanceLevels::SORELY_LACKING)
    {
        if(afflictions.Find(AfflictionTypes::HUNGER) == nullptr)
        {
            *afflictions.Add() = AfflictionFactory::Get()->Create(AfflictionTypes::HUNGER);
        }
    }

    if(growth >= growthThreshold)
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

    if(GetStock(ResourceTypes::METAL) < storage / 2)
        return;

    for(auto &link : links)
    {
        if(link.HasShipped == true)
            continue;

        auto other = link.Path->GetOther(this);

        if(other->GetStock(ResourceTypes::METAL) > other->storage / 4 || other->GetStock(ResourceTypes::METAL) > GetStock(ResourceTypes::METAL))
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

        bool cannotSendToOther = other->GetStock(ResourceTypes::METAL) > other->storage / 4 || other->GetStock(ResourceTypes::METAL) > GetStock(ResourceTypes::METAL);
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

    resourceHandler.Get(ResourceTypes::METAL)->Storage -= VOLUME_PER_SHIPMENT;

    shipment.To->ReceiveTransport();

    shipment.To = nullptr;
}

void Settlement::ReceiveTransport()
{
    resourceHandler.Get(ResourceTypes::METAL)->Storage += VOLUME_PER_SHIPMENT - SHIPMENT_VOLUME_LOSS;
}

void Settlement::UpdatePolitics()
{
    if(polity->GetRuler() == this)
        return;

    if(faction != nullptr)
        return;

    auto pathData = utility::Pathfinder <WorldTile>::Get()->FindPathToSettlement(this, polity->GetRuler());
    if(pathData.Complexity > 15)
    {
        faction = polity->FindFaction(this);
        /*independenceDrive++;

        if(independenceDrive > 100)
        {
            independenceDrive = 0;
            WorldScene::Get()->SplitPolity(this);
        }*/
    }
}