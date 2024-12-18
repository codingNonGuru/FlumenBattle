#include <mutex>

#include "FlumenCore/Utility/Utility.hpp"
#include "FlumenCore/Container/Map.hpp"

#include "Settlement.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/WorldBiome.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/WorldMap.h"
#include "FlumenBattle/World/WorldController.h"
#include "FlumenBattle/World/Polity/Polity.h"
#include "FlumenBattle/World/Polity/Faction.h"
#include "FlumenBattle/World/Settlement/Affliction.h"
#include "FlumenBattle/World/Settlement/SettlementEvent.h"
#include "FlumenBattle/World/Settlement/SettlementProduction.h"
#include "FlumenBattle/World/Settlement/Condition.h"
#include "FlumenBattle/World/Settlement/Building.h"
#include "FlumenBattle/World/Group/GroupDynamics.h"
#include "FlumenBattle/World/Settlement/Path.h"
#include "FlumenBattle/World/SimulationMap.h"
#include "FlumenBattle/World/SimulationDomain.h"
#include "FlumenBattle/Utility/Pathfinder.h"
#include "FlumenBattle/Utility/SettlementPathfinder.h"
#include "FlumenBattle/Utility/Utility.h"
#include "FlumenBattle/Config.h"
#include "FlumenBattle/ThreadedResourceHandler.h"
#include "FlumenBattle/Race.h"

using namespace world::settlement;

#define BORDER_GROWTH_THRESHOLD 400

bool Link::operator== (const settlement::Path &path) const 
{
    return *Path == path;
}

void Settlement::Initialize(Word name, Color banner, world::WorldTile *location, const Race *race)
{
    this->location = location;

    this->race = race;

    SetupSimulation();

    this->groupDynamics->Initialize(*this);

    this->buildingManager->Initialize(this);

    this->resourceHandler.Initialize();

    this->popHandler.Initialize();

    this->tradeHandler.Initialize();

    this->name = name;

    this->banner = banner;

    this->population = 1;

    this->growth = 0;

    this->cultureGrowth = 0;

    this->tradeDestination = nullptr;

    *this->currentProduction = SettlementProductionFactory::Get()->Create(ProductionOptions::NONE);

    auto tile = tiles.Add();
    tile->Tile = location;
    tile->IsWorked = true;
    tile->IsBuilt = false;

    auto nearbyTiles = location->GetNearbyTiles(1);
    for(auto &nearbyTile : nearbyTiles)
    {
        if(nearbyTile == this->location)
            continue;

        tile = tiles.Add();
        tile->Tile = nearbyTile;
        tile->IsWorked = false;
        tile->IsBuilt = false;
    }

    for(auto &tile : tiles)
    {
        tile.Tile->AssertOwnership(this);
    }

    AddBuilding(BuildingTypes::HOUSING);

    AddBuilding(BuildingTypes::WALLS);

    AddBuilding(BuildingTypes::KEEP);

    this->needsToReorganizeWork = true;
}

void Settlement::AddPath(Path *path) 
{
    *links.Add() = {path, path->GetOther(this)};
}

void Settlement::UpdateColonialMap()
{
    auto &interestMap = polity->GetInterestMap();
    auto centerMapping = interestMap.GetTile(location->HexCoordinates);

    for(int i = MINIMUM_COLONIZATION_RANGE; i <= MAXIMUM_COLONIZATION_RANGE; ++i)
    {
        auto &mappingRing = interestMap.GetTileRing(centerMapping, i);
        for(auto &mapping : mappingRing)
        {
            if(mapping->GetOwner() != this)
                continue;

            auto tile = location->GetMap()->GetTile(mapping->Coordinates);
            if(tile->HasBiome(WorldBiomes::STEPPE) == true && tile->IsBorderingOwnedTile() == false)
            {
                hasAvailableColonySpots = true;
                return;
            }
        }
    }

    hasAvailableColonySpots = false;
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
    static std::mutex mutex;
    mutex.lock();

    candidateTiles.Reset();

    auto bestChance = INT_MAX;
    WorldTile *bestTile = nullptr;
    for(int i = MINIMUM_COLONIZATION_RANGE; i <= MAXIMUM_COLONIZATION_RANGE; ++i)
    {
        auto tileRing = location->GetTileRing(i);
        for(auto &tile : tileRing)
        {
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
        mutex.unlock();
        return nullptr;
    }

    for(int i = MINIMUM_COLONIZATION_RANGE; i <= MAXIMUM_COLONIZATION_RANGE; ++i)
    {
        auto tileRing = location->GetTileRing(i);
        for(auto &tile : tileRing)
        {
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

    mutex.unlock();

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

void Settlement::GrowBorders() 
{
    if(tiles.GetSize() == tiles.GetCapacity())
        return;

    if(areNearbyTilesTaken == true)
        return;

    static std::mutex mutex;

    mutex.lock();

    auto findNewTileInRing = [this] (int distance) -> WorldTile *
    {
        auto tileRing = location->GetTileRing(distance);

        auto findNewTile = [this, &tileRing] (world::WorldBiomes biomeType) -> WorldTile *
        {
            for(auto &tile : tileRing)
            {
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

        return newTile;
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

    mutex.unlock();
}

struct NecessityMap
{
    container::StaticMap <int, ProductionOptions> Factors {16};

    container::Array <ProductionOptions> ExcludedOptions {16};
};

void Settlement::DecideProduction()
{
    static const ProductionOptions options[] = {
        //ProductionOptions::PATROL, 
        ProductionOptions::SETTLERS, 
        ProductionOptions::GARRISON,
        /*ProductionOptions::IRRIGATION, 
        ProductionOptions::LIBRARY,
        ProductionOptions::WALLS,
        ProductionOptions::SEWAGE,
        ProductionOptions::HOUSING,
        ProductionOptions::FARM*/
        };

    auto resourceHandler = game::ThreadedResourceHandler <NecessityMap>::Get();
    const auto necessityMap = resourceHandler->GetUsableResource();

    necessityMap->Factors.Reset();

    necessityMap->ExcludedOptions.Reset();

    for(auto &option : options)
    {
        auto necessity = SettlementProduction::GetNecessity(*this, option);
        
        *necessityMap->Factors.Add(option) = necessity;
    }

    auto findHighestViableOption = [&] -> container::StaticMap <int, ProductionOptions>::Iterator
    {
        auto chosenKey = ProductionOptions::NONE;
        auto highestValue = INT_MIN;

        auto value = necessityMap->Factors.GetStart();
        for(auto key = necessityMap->Factors.GetFirstKey(); key != necessityMap->Factors.GetLastKey(); ++key, ++value)
        {
            if(*value == 0)
                continue;

            bool isExcluded = false;
            for(auto &option : necessityMap->ExcludedOptions)
            {
                if(*key == option)
                {
                    isExcluded = true;
                    break;
                }
            }

            if(isExcluded == true)
                continue;

            if(*value > highestValue)
            {
                highestValue = *value;
                chosenKey = *key;
            }
        }

        return {highestValue, chosenKey};
    };

    while(true)
    {
        auto proposedOption = findHighestViableOption();

        if(proposedOption.Key_ == ProductionOptions::NONE)
        {
            *currentProduction = SettlementProductionFactory::Get()->Create(ProductionOptions::NONE);
            break;
        }
        else
        {
            auto inquiry = SettlementProduction::CanProduce(*this, proposedOption.Key_);
            if(inquiry.CanProduce == true)
            {
                *currentProduction = SettlementProductionFactory::Get()->Create(proposedOption.Key_, inquiry.Data);
                break;
            }
            else
            {
                *necessityMap->ExcludedOptions.Add() = proposedOption.Key_;
            }
        }
    }
}

void Settlement::SetProduction(ProductionOptions option)
{
    auto inquiry = SettlementProduction::CanProduce(*this, option);
    if(inquiry.CanProduce == false)
        return;

    *currentProduction = SettlementProductionFactory::Get()->Create(option, inquiry.Data);
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

const container::Pool <Building> &Settlement::GetBuildings() const 
{
    return buildingManager->GetBuildings();
}

bool Settlement::HasBuilding(BuildingTypes type) const
{
    return buildingManager->HasBuilding(type);
}

const Building &Settlement::GetBuilding(BuildingTypes type) const
{
    return buildingManager->GetBuilding(type);
}

int Settlement::GetBuildingCount(BuildingTypes type) const
{
    return buildingManager->GetBuildingCount(type);
}

int Settlement::GetTotalBuildingCount() const
{
    return buildingManager->GetBuildingCount();
}

const container::Array <Building *> &Settlement::GetBuildingsThatProduce(ResourceTypes resource) const
{
    return buildingManager->GetBuildingsThatProduce(resource);
}

AbundanceLevels Settlement::GetHousingAdequacy() const
{
    auto housingAmount = GetBuildingCount(BuildingTypes::HOUSING);

    if(housingAmount > population)
        return AbundanceLevels::ENOUGH;
    else if(housingAmount == population)
        return AbundanceLevels::BARELY_AVAILABLE;
    else if(housingAmount * 2 > population)
        return AbundanceLevels::LACKING;
    else
        return AbundanceLevels::SORELY_LACKING;
}

Integer Settlement::GetIndustrialProduction() const
{
    auto production = GetPopulation();

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

Integer Settlement::GetFreeWorkerCount() const
{
    auto buildingPersonnelCount = 0;
    for(auto &building : GetBuildings())
    {
        buildingPersonnelCount += building.GetPersonnelCount();
    }

    auto populationWithoutTileWorkers = 1 + population - GetWorkedTiles();

    return populationWithoutTileWorkers - buildingPersonnelCount;
}

bool Settlement::IsTileImproved(WorldTile* tile) const
{
    for(auto &settlementTile : tiles)
    {
        if(settlementTile.Tile == tile && settlementTile.IsBuilt == true)
            return true;
    }
    
    return false;
}

Settlement *Settlement::GetRuler() const
{
    return polity->GetRuler();
}

int Settlement::GetModifier(Modifiers modifier) const
{
    return modifierManager.GetAmount(modifier);
}

const Pool <Condition> &Settlement::GetConditions() const
{
    return conditionManager->GetConditions();
}

world::SimulationLevels Settlement::GetSimulationLevel() const
{
    return simulationLevel;
}

bool Settlement::IsDefended() const
{
    return groupDynamics->GetGarrisonStrength() != 0;
}

int Settlement::GetWallsLevel() const
{
    return GetBuildingCount(BuildingTypes::WALLS);
}

int Settlement::GetDefenceSneakDC() const
{
    return 12;
}

int Settlement::GetBribeGarrisonDC() const
{
    return 15;
}

int Settlement::GetMulePrice() const
{
    auto foodPrice = GetResourcePrice(ResourceTypes::FOOD);

    return foodPrice * 4 + 30;
}

bool Settlement::IsPlayerControlled() const
{
    return polity->IsPlayerControlled();
}

int Settlement::GetStandingBuildingCount() const
{
    return buildingManager->GetStandingBuildingCount();
}

bool Settlement::IsLootable() const
{
    if(IsDefended() == true)
        return false;

    if(buildingManager->GetStandingBuildingCount() > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Settlement::IsPillageable() const
{
    if(IsDefended() == true)
        return false;

    if(buildingManager->GetBuildingCount() > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int Settlement::GetLootDC() const
{
    auto bonus = GetModifier(Modifiers::LOOT_DC_BONUS);
    
    return 10 + bonus;
}

int Settlement::GetPillageDC() const
{
    auto bonus = GetModifier(Modifiers::PILLAGE_DC_BONUS);
    
    return 10 + bonus;
}

int Settlement::Loot(bool hasSparedBuilding, int requestedFood)
{
    if(hasSparedBuilding == false)
    {
        auto &building = buildingManager->GetRandomStandingBuilding();
        building.Damage();
    }

    if(requestedFood > GetResource(ResourceTypes::FOOD)->Storage)
    {
        requestedFood = GetResource(ResourceTypes::FOOD)->Storage;
    }

    GetResource(ResourceTypes::FOOD)->Storage -= requestedFood;

    return requestedFood;
}

void Settlement::Pillage(int buildingCount)
{
    for(int i = 0; i < buildingCount; ++i)
    {
        auto &building = buildingManager->GetRandomBuilding();
        building.Destroy();

        if(buildingManager->GetBuildingCount() == 0)
        {
            break;
        }
    }
}

void Settlement::SetupSimulation()
{
    simulationDomain = world::SimulationMap::Get()->GetDomain(location);

    simulationLevel = simulationDomain->Level;

    timeSinceSimulationChange = 0;
}

void Settlement::RemoveGroup(const group::Group &group)
{
    groupDynamics->RemoveGroup(group);
}

void Settlement::SetPolity(polity::Polity *polity)
{
    this->polity = polity;
}

void Settlement::KillPopulation()
{
    auto freeWorkerCount = GetFreeWorkerCount();

    population--;
    if(population < 0)
    {
        population = 0;
    }

    if(freeWorkerCount > 0)
        return;

    needsToReorganizeWork = true;

    for(auto &tile : tiles)
    {
        if(tile.IsWorked == true && tile.Tile != location)
        {
            tile.IsWorked = false;
            return;
        }
    }
}

void Settlement::StrengthenPatrol()
{
    groupDynamics->AddPatrol(*this);
}

void Settlement::StrengthenGarrison()
{
    groupDynamics->AddGarrison(*this);
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

bool Settlement::HireWorker(Building *building)
{
    if(building->GetOutputResource().Resource == world::settlement::ResourceTypes::NONE)
        return false;

    if(building->GetAmount() == building->GetPersonnelCount())
        return false;

    if(GetFreeWorkerCount() == 0)
        return false;

    building->AddPersonnel();

    return true;
}

void Settlement::FireAllWorkers()
{
    buildingManager->RemovePersonnel();

    for(auto &tile : tiles)
    {
        if(tile.Tile == GetLocation())
            continue;

        tile.IsWorked = false;
    }
}

void Settlement::ProcessEarthquake(const disaster::Earthquake &earthquake)
{
    BuildingDamager::DamageBuildings(earthquake, *buildingManager);

    BuildingDamager::DamageImprovements(earthquake, *this);
}

void Settlement::Update()
{
    static const auto &worldTime = world::WorldScene::Get()->GetTime();

    buildingManager->Update();

    auto updateModifiers = [this]
    {
        conditionManager->Update();

        modifierManager.ClearModifiers();

        if(IsHappy() == true)
        {
            AddCondition({settlement::Conditions::HAPPINESS, 1, 1});
        }

        polity->ApplyTechnologyModifiers(this);

        conditionManager->ApplyModifiers(*this);

        buildingManager->ApplyModifiers(*this);

        race->ApplyModifiers(*this);
    };

    updateModifiers();

    resourceHandler.Update(*this);

    auto foodSecurity = resourceHandler.Get(ResourceTypes::FOOD)->ShortTermAbundance;
    auto housingAdequacy = GetHousingAdequacy();

    auto addedGrowth = [&]
    {
        auto growthFromFood = [&]
        {
            switch(foodSecurity)
            {
            case AbundanceLevels::CORNUCOPIA:
                return 5;
            case AbundanceLevels::ABUNDANT:
                return 4;
            case AbundanceLevels::ENOUGH:
                return 3;
            case AbundanceLevels::BARELY_AVAILABLE:
                return 0;
            case AbundanceLevels::LACKING:
                return -5;
            case AbundanceLevels::SORELY_LACKING:
                return -7;
            }
        } ();

        auto growthFromHousing = [&]
        {
            switch(housingAdequacy)
            {
            case AbundanceLevels::ENOUGH:
                return 1;
            case AbundanceLevels::BARELY_AVAILABLE:
                return 0;
            case AbundanceLevels::LACKING:
                return -1;
            case AbundanceLevels::SORELY_LACKING:
                return -3;
            }
        } ();

        auto modifier = GetModifier(Modifiers::POPULATION_GROWTH_RATE);

        return growthFromFood + growthFromHousing + modifier;
    } ();

    growth += addedGrowth;
    if(growth >= growthThreshold)
    {
        growth = 0;
        population++;

        needsToReorganizeWork = true;
    }
    else if(growth < 0)
    {
        growth = 0;
    }

    cultureGrowth++;
    if(cultureGrowth >= BORDER_GROWTH_THRESHOLD)
    {
        cultureGrowth = 0;

        needsToReorganizeWork = true;

        GrowBorders();
    }

    if(foodSecurity == AbundanceLevels::LACKING || foodSecurity == AbundanceLevels::SORELY_LACKING)
    {
        if(afflictions.Find(AfflictionTypes::HUNGER) == nullptr)
        {
            *afflictions.Add() = AfflictionFactory::Get()->Create(AfflictionTypes::HUNGER);
        }
    }

    if(worldTime.MinuteCount == 0 && population > 3)
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

    //WorkNewTile();


    currentProduction->AddProgress(currentProduction->Is(ProductionOptions::NONE) ? 1 : GetIndustrialProduction());
    
    if(currentProduction->IsDone())
    {
        currentProduction->Finish(*this);
            
        *currentProduction = SettlementProductionFactory::Get()->Create(ProductionOptions::NONE);
    }

    if(worldTime.MinuteCount == 0)
    {
        groupDynamics->Update(*this);

        if(simulationLevel != simulationDomain->Level)
        {
            timeSinceSimulationChange++;

            static const auto threshold = engine::ConfigManager::Get()->GetValue(game::ConfigValues::SIMULATION_CHANGE_DELAY).Integer;
            if(timeSinceSimulationChange > threshold)
            {
                timeSinceSimulationChange = 0;

                simulationLevel = simulationDomain->Level;

                hasUsedSimulationChange = false;
            }
        }
    }
}

void Settlement::PrepareTransport()
{
    tradeHandler.PrepareTransport(*this);
}

void Settlement::SendTransport()
{
    tradeHandler.SendTransport(*this);
}

void Settlement::FinishTradeUpdate()
{
    tradeHandler.FinishUpdate(*this);
}

void Settlement::ReceiveTransport(ResourceTypes resource, int amount)
{
    resourceHandler.Get(resource)->Storage += amount;
}

void Settlement::UpdatePolitics()
{
    if(polity->GetRuler() == this)
        return;

    if(faction != nullptr)
        return;

    auto modifier = modifierManager.GetAmount(Modifiers::FACTION_JOIN_INTERDICTION);
    if(modifier.IsValid == true)
        return;

    auto distanceModifier = modifierManager.GetAmount(Modifiers::DISTANCE_TO_CAPITAL_INDEPENDENCE_LIMIT);

    if(distanceToCapital > 12 + distanceModifier)
    {
        faction = polity->FindFaction(this);
    }
}

void Settlement::UpdateDistanceToCapital()
{
    if(polity->GetRuler() == this)
    {
        distanceToCapital = 0;
        return;
    }

    auto pathData = utility::SettlementPathfinder::Get()->FindPathToSettlement(this, polity->GetRuler());
    distanceToCapital = pathData.Complexity;
}