#include <mutex>

#include "FlumenCore/Utility/Utility.hpp"
#include "FlumenCore/Container/Map.hpp"

#include "Settlement.h"
#include "FlumenBattle/World/Settlement/SettlementTile.h"
#include "FlumenBattle/World/Settlement/TileImprovement.h"
#include "FlumenBattle/World/Tile/WorldTile.h"
#include "FlumenBattle/World/Tile/WorldBiome.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Tile/WorldMap.h"
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
#include "FlumenBattle/World/Settlement/ExplorationHandler.h"
#include "FlumenBattle/World/WorldUpdateHandler.h"

using namespace world::settlement;

#define BORDER_GROWTH_THRESHOLD 1000

bool Link::operator== (const settlement::Path &path) const 
{
    return *Path == path;
}

void Settlement::Initialize(Word name, Color banner, tile::WorldTile *location, const Race *race)
{
    this->isValid = true;

    this->foundationDate = WorldScene::Get()->GetTime().GetTickCount();

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

    this->cultureGrowth = 0;

    this->tradeDestination = nullptr;

    this->currentExploration = {nullptr, 0};

    this->currentImprovement = {nullptr};

    *this->currentProduction = SettlementProductionFactory::Get()->Create(ProductionOptions::NONE);

    auto tile = tiles.Add();
    tile->Tile = location;
    tile->IsWorked = true;
    tile->ResetImprovement();

    auto nearbyTiles = location->GetNearbyTiles(1);
    for(auto &nearbyTile : nearbyTiles)
    {
        if(nearbyTile == this->location)
            continue;

        tile = tiles.Add();
        tile->Tile = nearbyTile;
        tile->IsWorked = false;
        tile->ResetImprovement();
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

void Settlement::RemoveLink(Settlement *settlement)
{
    links.Remove(settlement);
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

static Array <world::tile::WorldTile *> candidateTiles = Array <world::tile::WorldTile *> (128);

world::tile::WorldTile * Settlement::FindColonySpot()
{
    candidateTiles.Reset();

    auto bestChance = INT_MAX;
    tile::WorldTile *bestTile = nullptr;
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
        //ProductionOptions::SETTLERS, 
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

    *currentProduction = SettlementProductionFactory::Get()->Create(option);
}

Color Settlement::GetRulerBanner() const
{
    return polity->GetRuler()->GetBanner();
}

world::tile::WorldTile * Settlement::GetLocation() const
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

    if(housingAmount > GetPopulation())
        return AbundanceLevels::ENOUGH;
    else if(housingAmount == GetPopulation())
        return AbundanceLevels::BARELY_AVAILABLE;
    else if(housingAmount * 2 > GetPopulation())
        return AbundanceLevels::LACKING;
    else
        return AbundanceLevels::SORELY_LACKING;
}

Integer Settlement::GetIndustrialProduction() const
{
    if(IsAbandoned() == true)
        return 1;

    auto population = GetPopulation();

    return 3 + (population - 1) * 2;
}

Integer Settlement::GetScienceProduction() const
{
    if(IsAbandoned() == true)
        return 0;

    auto production = GetPopulation() >= 10 ? 2 : 1;
    production += GetModifier(Modifiers::SCIENCE_PRODUCTION);

    return production;
}

bool Settlement::CanGrowBorders() const
{
    return cultureGrowth == BORDER_GROWTH_THRESHOLD;
}

bool Settlement::CanExpandHere(tile::WorldTile *tile) const
{
    if(tile->IsOwned() == true)
        return false;

    if(HasExplored(tile) == false)  
        return false;

    static const auto BORDER_EXPANSION_MAX_DISTANCE = engine::ConfigManager::Get()->GetValue(game::ConfigValues::BORDER_EXPANSION_MAX_DISTANCE).Integer;

    if(GetLocation()->GetDistanceTo(*tile) > BORDER_EXPANSION_MAX_DISTANCE)
        return false;

    bool hasAtLeastOneNeighbour = false;

    auto nearbyTiles = tile->GetNearbyTiles();
    for(auto &neighbour : nearbyTiles)
    {
        if(neighbour->GetOwner() == this)
        {
            hasAtLeastOneNeighbour = true;
            break;
        }
    }

    if(hasAtLeastOneNeighbour == false)
        return false;

    return true;
}

bool Settlement::CanSettleHere(tile::WorldTile *tile) const
{
    if(tile->HasRelief(WorldReliefs::SEA) == true)
        return false;

    if(tile->IsBorderingOwnedTile() == true)
        return false;

    auto distance = GetLocation()->GetDistanceTo(*tile);
    if(distance < MINIMUM_COLONIZATION_RANGE || distance > MAXIMUM_COLONIZATION_RANGE)
        return false;

    return true;
}

int Settlement::GetExpansionCost(tile::WorldTile *tile) const
{
    auto distance = GetLocation()->GetDistanceTo(*tile);

    if(distance == 0)
    {
        return 0;
    }
    else if(distance == 1)
    {
        return 400;
    }
    else if(distance == 2)
    {
        return 700;
    }
    else if(distance >= 3)
    {
        return 700 + (distance - 2) * 200;
    }
}

bool Settlement::CanAffordToExpandHere(tile::WorldTile *tile) const
{
    return cultureGrowth >= GetExpansionCost(tile);
}

float Settlement::GetCultureProgress() const
{
    return (float)cultureGrowth / (float)BORDER_GROWTH_THRESHOLD;
}

Integer Settlement::GetPopulation() const 
{
    return popHandler.GetPopulation();
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

    auto populationWithoutTileWorkers = 1 + GetPopulation() - GetWorkedTiles();

    return populationWithoutTileWorkers - buildingPersonnelCount;
}

bool Settlement::IsTileImproved(tile::WorldTile* tile) const
{
    for(auto &settlementTile : tiles)
    {
        if(settlementTile.Tile == tile && settlementTile.IsImproved() == true)
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

void Settlement::IncreasePopulation()
{
    popHandler.IncreasePopulation(this);
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

void Settlement::RemoveGroup(const group::GroupCore &group)
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

    popHandler.KillPopulation();

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

bool Settlement::IsAbandoned() const
{
    return popHandler.IsSettlementAbandoned();
}

int Settlement::GetAbandonmentSeverity() const
{
    return popHandler.GetAbandonmentSeverity();
}

int Settlement::GetHighestPopulationEver() const 
{
    return popHandler.GetHighestPopulationEver();
}

bool Settlement::IsRuins() const
{
    return popHandler.IsSettlementRuins();
}

bool Settlement::IsCompletelyGone() const
{
    return popHandler.IsSettlementCompletelyGone();
}

void Settlement::Update()
{
    if(isValid == false)
    {
        turnsUntilDeletion--;
        return;
    }

    static const auto &worldTime = world::WorldScene::Get()->GetTime();

    popHandler.CheckAbandonment(this);

    //StrengthenPatrol();

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

    if(popHandler.IsSettlementAbandoned() == true)
    {
        return;
    }

    resourceHandler.Update(*this);

    popHandler.UpdateGrowth(this);

    cultureGrowth++;
    if(cultureGrowth >= BORDER_GROWTH_THRESHOLD)
    {
        cultureGrowth = BORDER_GROWTH_THRESHOLD;
    }

    auto foodSecurity = GetResource(ResourceTypes::FOOD)->ShortTermAbundance;
    if(foodSecurity == AbundanceLevels::LACKING || foodSecurity == AbundanceLevels::SORELY_LACKING)
    {
        if(afflictions.Find(AfflictionTypes::HUNGER) == nullptr)
        {
            *afflictions.Add() = AfflictionFactory::Get()->Create(AfflictionTypes::HUNGER);
        }
    }

    if(worldTime.MinuteCount == 0 && GetPopulation() > 3)
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

        GetPolity()->RegisterProductionFinished(this);
            
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
    if(isValid == false)
        return;

    if(IsAbandoned() == true)
        return;

    tradeHandler.PrepareTransport(*this);
}

void Settlement::SendTransport()
{
    if(isValid == false)
        return;

    if(IsAbandoned() == true)
        return;

    tradeHandler.SendTransport(*this);
}

void Settlement::FinishTradeUpdate()
{
    if(isValid == false)
        return;

    tradeHandler.FinishUpdate(*this);
}

void Settlement::ReceiveTransport(ResourceTypes resource, int amount)
{
    resourceHandler.Get(resource)->Storage += amount;
}

void Settlement::UpdatePolitics()
{
    if(isValid == false)
        return;

    if(IsAbandoned() == true)
        return;

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

void Settlement::AddSettlers() 
{
    groupDynamics->AddSettlers(*this);
}

void Settlement::RemoveSettlers() 
{
    groupDynamics->RemoveSettlers();
}

bool Settlement::HasMaximumSettlers() const 
{
    return groupDynamics->HasMaximumSettlers();
}

bool Settlement::HasAnySettlers() const 
{
    return groupDynamics->HasAnySettlers();
}

bool Settlement::IsExploring(tile::WorldTile *tile) const
{
    return currentExploration.Tile == tile;
}

bool Settlement::IsExploring() const
{
    return currentExploration.Tile != nullptr;
}

bool Settlement::HasExplored(tile::WorldTile *tile) const
{
    auto exploration = finishedExplorations.Find(tile);
    return exploration != nullptr;
}

container::Array <world::tile::WorldTile *> &Settlement::GetExploredTiles() const
{
    static const auto EXPLORATIONS_PER_SETTLEMENT = engine::ConfigManager::Get()->GetValue(game::ConfigValues::EXPLORATIONS_PER_SETTLEMENT).Integer;

    static auto exploredTiles = container::Array <tile::WorldTile *> (EXPLORATIONS_PER_SETTLEMENT);

    exploredTiles.Reset();

    for(auto &exploration : finishedExplorations)
    {
        *exploredTiles.Add() = exploration.Tile;
    }

    return exploredTiles;
}

bool Settlement::CanExploreHere(tile::WorldTile *tile) const
{
    if(tile->IsOwned() == true)
        return false;

    if(GetLocation()->GetDistanceTo(*tile) > MAXIMUM_COLONIZATION_RANGE)
        return false;

    if(finishedExplorations.IsFull() == true)
        return false;

    return true;
}

bool Settlement::CanImproveHere(tile::WorldTile *tile, TileImprovements type) const
{
    if(tile->IsOwned() == false)
        return false;

    if(tile == GetLocation())
        return false;

    if(tile->GetOwner() != this)
        return false;

    auto improvementType = TileImprovementFactory::Get()->BuildImprovementType(type);
    auto resource = improvementType->Resource;
    if(tile->GetResource(resource) == 0)
        return false;

    if(tile->GetOwner() == this && HasImprovement(tile, type) == true)
        return false;

    return true;
}

void Settlement::StartImprovingTile(tile::WorldTile *tile, TileImprovements improvement)
{
    currentImprovement = {tiles.Find(tile), improvement};

    SetProduction(ProductionOptions::FARM);
}

bool Settlement::IsImprovingTile(SettlementTile *tile, TileImprovements improvement) const
{
    return currentImprovement.Tile == tile && currentImprovement.ImprovementType == improvement;
}

bool Settlement::IsImprovingTile(tile::WorldTile *tile, TileImprovements improvement) const
{
    if(currentImprovement.Tile == nullptr)
        return false;

    return currentImprovement.Tile->Tile == tile && currentImprovement.ImprovementType == improvement;
}

void Settlement::CancelImproving() 
{
    currentImprovement = {nullptr};

    SetProduction(ProductionOptions::NONE);
}

bool Settlement::HasImprovement(tile::WorldTile *tile, TileImprovements improvement) const
{
    auto settlementTile = tiles.Find(tile);

    if(settlementTile->GetImprovementType() == nullptr)
        return false;

    return settlementTile->GetImprovementType()->Type == improvement;
}

bool Settlement::HasImprovement(SettlementTile *tile, TileImprovements improvement) const
{
    return tile->GetImprovementType()->Type == improvement;
}

void Settlement::StartExploring(tile::WorldTile *tile)
{
    currentExploration = {tile, 0};
}

void Settlement::StopExploring()
{
    currentExploration.Tile = nullptr;
}

void Settlement::RemoveFinishedExploration(tile::WorldTile *tile)
{
    finishedExplorations.Remove(tile);
}

#define EXPLORATION_SUCCESS_THRESHOLD 100

ExplorationReward lastExplorationReward;

void Settlement::AddExplorationProgress(int progress)
{
    if(currentExploration.Tile == nullptr)
        return;

    currentExploration.Progress += progress;
    if(currentExploration.Progress > EXPLORATION_SUCCESS_THRESHOLD)
    {
        *finishedExplorations.Add() = {currentExploration.Tile};

        auto reward = ExplorationHandler::Get()->GetReward(currentExploration.Tile);

        WorldUpdateHandler::Get()->LoadExplorationRewardData({reward, this, currentExploration.Tile});

        currentExploration = {nullptr, 0};
    }
}

void Settlement::MarkForDeletion()
{
    isValid = false;

    turnsUntilDeletion = 6;

    groupDynamics->MarkForDeletion();
}

const ExplorationReward &Settlement::GetLastExplorationReward() const
{
    return lastExplorationReward;
}

float Exploration::GetProgressFactor() const
{
    return (float)Progress / (float)EXPLORATION_SUCCESS_THRESHOLD;
}