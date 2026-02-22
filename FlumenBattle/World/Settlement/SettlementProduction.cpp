#include <mutex>

#include "SettlementProduction.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/SettlementTile.h"
#include "FlumenBattle/World/Group/GroupDynamics.h"
#include "FlumenBattle/World/Tile/WorldTile.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Settlement/ProductionEvaluator.h"
#include "FlumenBattle/Race.h"
#include "FlumenBattle/Utility/Utility.h"
#include "FlumenBattle/Config.h"

using namespace world::settlement;

#define COLONIZATION_DC 1

SettlementProduction SettlementProductionFactory::Create(ProductionOptions option, ProductionData data)
{
    return {BuildProductionType(option), 0, data};
}

int SettlementProduction::GetNecessity(Settlement &settlement, ProductionOptions option)
{
    return ProductionEvaluator::GetNecessity(settlement, option);
}

ProductionInquiry SettlementProduction::CanProduce(Settlement &settlement, ProductionOptions option)
{
    switch(option)
    {
    case ProductionOptions::SETTLERS:
    {
        auto colonySpot = settlement.FindColonySpot();
        if(colonySpot == nullptr)
        {
            return {false};
        }
        else
        {
            return {true, {colonySpot}};
        }
    }
    case ProductionOptions::GARRISON:
    {
        return {settlement.GetGroupDynamics().HasMaximumGarrisons() == false};
    }
    case ProductionOptions::FARM:
    {
        return {true};
        /*SettlementTile *improvementTarget = nullptr;
        for(auto &tile : settlement.GetTiles())
        {
            if(tile.Tile->HasBiome(world::WorldBiomes::STEPPE) && tile.IsImproved() == false && tile.Tile != settlement.GetLocation())
            {
                improvementTarget = &tile;
            }
        }

        if(improvementTarget == nullptr)
        {
            return {false};
        }
        else
        {
            return {true, {improvementTarget}};
        }*/
    }
    case ProductionOptions::SEWAGE:
        return {settlement.HasBuilding(BuildingTypes::SEWAGE) == false};
    case ProductionOptions::IRRIGATION:
        return {settlement.HasBuilding(BuildingTypes::IRRIGATION) == false};
    case ProductionOptions::LIBRARY:
        return {settlement.HasBuilding(BuildingTypes::LIBRARY) == false};
    case ProductionOptions::WALLS:
        static const auto MAXIMUM_WALLS_LEVEL = engine::ConfigManager::Get()->GetValue(game::ConfigValues::MAXIMUM_WALLS_LEVEL).Integer;

        return {settlement.GetBuildingCount(BuildingTypes::WALLS) < MAXIMUM_WALLS_LEVEL};
    case ProductionOptions::LUMBER_MILL:
        return {settlement.HasBuilding(BuildingTypes::LUMBER_MILL) == false};
    case ProductionOptions::CARPENTER:
        return {settlement.HasBuilding(BuildingTypes::CARPENTER) == false};
    case ProductionOptions::BAKERY:
        return {settlement.HasBuilding(BuildingTypes::BAKERY) == false};
    case ProductionOptions::MARKET:
        return {settlement.GetBuildingCount(BuildingTypes::MARKET) < 3};
    case ProductionOptions::WEAVING_MILL:
        return {settlement.HasBuilding(BuildingTypes::WEAVING_MILL) == false};
    case ProductionOptions::POTTERY:
        return {settlement.GetBuildingCount(BuildingTypes::POTTERY) < 3};
    case ProductionOptions::TAILORY:
        return {settlement.HasBuilding(BuildingTypes::TAILORY) == false};
    case ProductionOptions::KEEP:
        return {settlement.HasBuilding(BuildingTypes::KEEP) == false};
    default:
        return {true};
    }
}

const Word &SettlementProduction::GetName(ProductionOptions option)
{
    return SettlementProductionFactory::Get()->BuildProductionType(option)->Name;
}

const SettlementProductionType *SettlementProduction::GetType(ProductionOptions option)
{
    return SettlementProductionFactory::Get()->BuildProductionType(option);
}

const SettlementProductionType * SettlementProductionFactory::BuildProductionType(ProductionOptions option) const
{
    switch(option)
    {
        case ProductionOptions::PATROL:
            return BuildPatrolProduction();
        case ProductionOptions::GARRISON:
            return BuildGarrisonProduction();
        case ProductionOptions::FARM:
            return BuildFarmProduction();
        case ProductionOptions::SETTLERS:
            return BuildSettlersProduction();
        case ProductionOptions::SEWAGE:
            return BuildSewageProduction();
        case ProductionOptions::IRRIGATION:
            return BuildIrrigationProduction();
        case ProductionOptions::LIBRARY:
            return BuildLibraryProduction();
        case ProductionOptions::HOUSING:
            return BuildHousingProduction();
        case ProductionOptions::LUMBER_MILL:
            return BuildLumberMillProduction();
        case ProductionOptions::WEAVING_MILL:
            return BuildWeavingMillProduction();
        case ProductionOptions::POTTERY:
            return BuildPotteryProduction();
        case ProductionOptions::TAILORY:
            return BuildTailoryProduction();
        case ProductionOptions::CARPENTER:
            return BuildCarpenterProduction();
        case ProductionOptions::BAKERY:
            return BuildBakeryProduction();
        case ProductionOptions::MARKET:
            return BuildMarketProduction();
        case ProductionOptions::WALLS:
            return BuildWallsProduction();
        case ProductionOptions::KEEP:
            return BuildKeepProduction();
        case ProductionOptions::NONE:
            return BuildNoneProduction();
    }
}

const SettlementProductionType * SettlementProductionFactory::BuildPatrolProduction() const
{
    static const SettlementProductionType productionType = {ProductionOptions::PATROL, "Patrol", ProductionClasses::RECRUITMENT, 2000, &ProductionFinisher::FinishPatrol};
    return &productionType;
}

const SettlementProductionType * SettlementProductionFactory::BuildGarrisonProduction() const
{
    static const SettlementProductionType productionType = {ProductionOptions::GARRISON, "Garrison", ProductionClasses::RECRUITMENT, 2000, &ProductionFinisher::FinishGarrison};
    return &productionType;
}

const SettlementProductionType * SettlementProductionFactory::BuildFarmProduction() const
{
    static const SettlementProductionType productionType = {ProductionOptions::FARM, "Tile improvement", ProductionClasses::TILE_IMPROVEMENT, 3000, &ProductionFinisher::FinishFarm};
    return &productionType;
}

const SettlementProductionType * SettlementProductionFactory::BuildSettlersProduction() const
{
    static const SettlementProductionType productionType = {ProductionOptions::SETTLERS, "Settlers", ProductionClasses::RECRUITMENT, 3000, &ProductionFinisher::FinishSettlers};
    return &productionType;
}

const SettlementProductionType * SettlementProductionFactory::BuildSewageProduction() const
{
    static const SettlementProductionType productionType = {ProductionOptions::SEWAGE, "Sewage", ProductionClasses::BUILDING, 4000, &ProductionFinisher::FinishSewage};
    return &productionType;
}

const SettlementProductionType * SettlementProductionFactory::BuildIrrigationProduction() const
{
    static const SettlementProductionType productionType = {ProductionOptions::IRRIGATION, "Irrigation", ProductionClasses::BUILDING, 7000, &ProductionFinisher::FinishIrrigation};
    return &productionType;
}

const SettlementProductionType * SettlementProductionFactory::BuildLibraryProduction() const
{
    static const SettlementProductionType productionType = {ProductionOptions::LIBRARY, "Library", ProductionClasses::BUILDING, 5000, &ProductionFinisher::FinishLibrary};
    return &productionType;
}

const SettlementProductionType * SettlementProductionFactory::BuildWallsProduction() const
{
    static const SettlementProductionType productionType = {ProductionOptions::WALLS, "Walls", ProductionClasses::BUILDING, 5000, &ProductionFinisher::FinishWalls};
    return &productionType;
}

const SettlementProductionType * SettlementProductionFactory::BuildHousingProduction() const
{
    static const SettlementProductionType productionType = {ProductionOptions::HOUSING, "Housing", ProductionClasses::BUILDING, 3000, &ProductionFinisher::FinishHousing};
    return &productionType;
}

const SettlementProductionType *SettlementProductionFactory::BuildLumberMillProduction() const
{
    static const SettlementProductionType productionType = {ProductionOptions::LUMBER_MILL, "Lumber mill", ProductionClasses::BUILDING, 3000, &ProductionFinisher::FinishLumberMill};
    return &productionType;
}

const SettlementProductionType *SettlementProductionFactory::BuildWeavingMillProduction() const
{
    static const SettlementProductionType productionType = {ProductionOptions::WEAVING_MILL, "Weaving mill", ProductionClasses::BUILDING, 3000, &ProductionFinisher::FinishWeavingMill};
    return &productionType;
}

const SettlementProductionType *SettlementProductionFactory::BuildPotteryProduction() const
{
    static const SettlementProductionType productionType = {ProductionOptions::POTTERY, "Pottery", ProductionClasses::BUILDING, 3000, &ProductionFinisher::FinishPottery};
    return &productionType;
}

const SettlementProductionType *SettlementProductionFactory::BuildTailoryProduction() const
{
    static const SettlementProductionType productionType = {ProductionOptions::TAILORY, "Tailory", ProductionClasses::BUILDING, 3000, &ProductionFinisher::FinishTailory};
    return &productionType;
}

const SettlementProductionType *SettlementProductionFactory::BuildCarpenterProduction() const
{
    static const SettlementProductionType productionType = {ProductionOptions::CARPENTER, "Carpenter", ProductionClasses::BUILDING, 3000, &ProductionFinisher::FinishCarpenter};
    return &productionType;
}

const SettlementProductionType *SettlementProductionFactory::BuildBakeryProduction() const
{
    static const SettlementProductionType productionType = {ProductionOptions::BAKERY, "Bakery", ProductionClasses::BUILDING, 3000, &ProductionFinisher::FinishBakery};
    return &productionType;
}

const SettlementProductionType *SettlementProductionFactory::BuildMarketProduction() const
{
    static const SettlementProductionType productionType = {ProductionOptions::MARKET, "Market", ProductionClasses::BUILDING, 3000, &ProductionFinisher::FinishMarket};
    return &productionType;
}

const SettlementProductionType *SettlementProductionFactory::BuildKeepProduction() const
{
    static const SettlementProductionType productionType = {ProductionOptions::KEEP, "Keep", ProductionClasses::BUILDING, 3000, &ProductionFinisher::FinishKeep};
    return &productionType;
}

const SettlementProductionType * SettlementProductionFactory::BuildNoneProduction() const
{
    static const SettlementProductionType productionType = {ProductionOptions::NONE, "Nothing", ProductionClasses::NONE, 6, nullptr};
    return &productionType;
}

const container::Array <const SettlementProductionType *> &SettlementProductionFactory::GetRecruitmentOptions() const
{
    static const auto options = [&]
    {
        static container::Array <const SettlementProductionType *> options;

        for(int i = 0; i < (int)ProductionOptions::NONE; ++i)
        {
            auto type = BuildProductionType(ProductionOptions(i));
            if(type->Class == ProductionClasses::RECRUITMENT)
            {
                *options.Add() = type;
            }
        }

        return &options;
    } ();

    return *options;
}

const container::Array <const SettlementProductionType *> &SettlementProductionFactory::GetBuildingOptions() const
{
    static const auto options = [&]
    {
        static container::Array <const SettlementProductionType *> options;

        for(int i = 0; i < (int)ProductionOptions::NONE; ++i)
        {
            auto type = BuildProductionType(ProductionOptions(i));
            if(type->Class == ProductionClasses::BUILDING)
            {
                *options.Add() = type;
            }
        }

        return &options;
    } ();

    return *options;
}

void ProductionFinisher::FinishPatrol(Settlement &settlement)
{
    settlement.StrengthenPatrol();
}

void ProductionFinisher::FinishGarrison(Settlement &settlement)
{
    settlement.StrengthenGarrison();
}

void ProductionFinisher::FinishSewage(Settlement &settlement)
{
    settlement.AddBuilding(BuildingTypes::SEWAGE);
}      

void ProductionFinisher::FinishLibrary(Settlement &settlement)
{
    settlement.AddBuilding(BuildingTypes::LIBRARY);
}      

void ProductionFinisher::FinishLumberMill(Settlement &settlement)
{
    settlement.AddBuilding(BuildingTypes::LUMBER_MILL);
}

void ProductionFinisher::FinishWeavingMill(Settlement &settlement)
{
    settlement.AddBuilding(BuildingTypes::WEAVING_MILL);
}

void ProductionFinisher::FinishPottery(Settlement &settlement)
{
    settlement.AddBuilding(BuildingTypes::POTTERY);
}

void ProductionFinisher::FinishTailory(Settlement &settlement)
{
    settlement.AddBuilding(BuildingTypes::TAILORY);
}

void ProductionFinisher::FinishCarpenter(Settlement &settlement)
{
    settlement.AddBuilding(BuildingTypes::CARPENTER);
}

void ProductionFinisher::FinishBakery(Settlement &settlement)
{
    settlement.AddBuilding(BuildingTypes::BAKERY);
}

void ProductionFinisher::FinishMarket(Settlement &settlement)
{
    settlement.AddBuilding(BuildingTypes::MARKET);
}

void ProductionFinisher::FinishKeep(Settlement &settlement)
{
    settlement.AddBuilding(BuildingTypes::KEEP);
}

void ProductionFinisher::FinishWalls(Settlement &settlement)
{
    settlement.AddBuilding(BuildingTypes::WALLS);
}

void ProductionFinisher::FinishFarm(Settlement &settlement)
{
    settlement.currentImprovement.Tile->AddImprovement(settlement.currentImprovement.ImprovementType);

    settlement.currentImprovement = {nullptr};
    //auto target = settlement.buildingProduction->data.improvementTarget;
    //target->IsBuilt = true;
}      

void ProductionFinisher::FinishSettlers(Settlement &settlement)
{
    auto bonus = settlement.GetModifier(Modifiers::ALL_DICE_ROLLS);

    auto check = utility::RollD20Dice(COLONIZATION_DC, bonus);
    if(check.IsCriticalSuccess() == true)
    {
        settlement.AddSettlers();
    }
    else if(check.IsAnySuccess() == true)
    {
        settlement.AddSettlers();
        
        settlement.KillPopulation();
    }
    else if(check.IsRegularFailure() == true) 
    {
        
    }
    else
    {
        settlement.KillPopulation();
    }

    /*auto polity = settlement.polity;
    auto target = settlement.buildingProduction->data.colonizationTarget;

    if(target->IsBorderingOwnedTile() == true)
    {
        target = settlement.FindColonySpot();
    }

    static std::mutex mutex;
    mutex.lock();

    if(target != nullptr)
    {

    }

    mutex.unlock();*/
}

void ProductionFinisher::FinishIrrigation(Settlement &settlement)
{
    settlement.AddBuilding(BuildingTypes::IRRIGATION);
}      

void ProductionFinisher::FinishHousing(Settlement &settlement)
{
    settlement.AddBuilding(BuildingTypes::HOUSING);
}      