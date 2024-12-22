#include <mutex>

#include "SettlementProduction.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/SettlementTile.h"
#include "FlumenBattle/World/Group/GroupDynamics.h"
#include "FlumenBattle/World/WorldTile.h"
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

const SettlementProductionType * SettlementProductionFactory::BuildProductionType(ProductionOptions option)
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

const SettlementProductionType * SettlementProductionFactory::BuildPatrolProduction()
{
    static const SettlementProductionType productionType = {ProductionOptions::PATROL, "Patrol", 200, &ProductionFinisher::FinishPatrol};
    return &productionType;
}

const SettlementProductionType * SettlementProductionFactory::BuildGarrisonProduction()
{
    static const SettlementProductionType productionType = {ProductionOptions::GARRISON, "Garrison", 200, &ProductionFinisher::FinishGarrison};
    return &productionType;
}

const SettlementProductionType * SettlementProductionFactory::BuildFarmProduction()
{
    static const SettlementProductionType productionType = {ProductionOptions::FARM, "Tile improvement", 300, &ProductionFinisher::FinishFarm};
    return &productionType;
}

const SettlementProductionType * SettlementProductionFactory::BuildSettlersProduction()
{
    static const SettlementProductionType productionType = {ProductionOptions::SETTLERS, "Settlers", 100, &ProductionFinisher::FinishSettlers};
    return &productionType;
}

const SettlementProductionType * SettlementProductionFactory::BuildSewageProduction()
{
    static const SettlementProductionType productionType = {ProductionOptions::SEWAGE, "Sewage", 400, &ProductionFinisher::FinishSewage};
    return &productionType;
}

const SettlementProductionType * SettlementProductionFactory::BuildIrrigationProduction()
{
    static const SettlementProductionType productionType = {ProductionOptions::IRRIGATION, "Irrigation", 700, &ProductionFinisher::FinishIrrigation};
    return &productionType;
}

const SettlementProductionType * SettlementProductionFactory::BuildLibraryProduction()
{
    static const SettlementProductionType productionType = {ProductionOptions::LIBRARY, "Library", 500, &ProductionFinisher::FinishLibrary};
    return &productionType;
}

const SettlementProductionType * SettlementProductionFactory::BuildWallsProduction()
{
    static const SettlementProductionType productionType = {ProductionOptions::WALLS, "Walls", 500, &ProductionFinisher::FinishWalls};
    return &productionType;
}

const SettlementProductionType * SettlementProductionFactory::BuildHousingProduction()
{
    static const SettlementProductionType productionType = {ProductionOptions::HOUSING, "Housing", 300, &ProductionFinisher::FinishHousing};
    return &productionType;
}

const SettlementProductionType *SettlementProductionFactory::BuildLumberMillProduction()
{
    static const SettlementProductionType productionType = {ProductionOptions::LUMBER_MILL, "Lumber mill", 300, &ProductionFinisher::FinishLumberMill};
    return &productionType;
}

const SettlementProductionType *SettlementProductionFactory::BuildWeavingMillProduction()
{
    static const SettlementProductionType productionType = {ProductionOptions::WEAVING_MILL, "Weaving mill", 300, &ProductionFinisher::FinishWeavingMill};
    return &productionType;
}

const SettlementProductionType *SettlementProductionFactory::BuildPotteryProduction()
{
    static const SettlementProductionType productionType = {ProductionOptions::POTTERY, "Pottery", 300, &ProductionFinisher::FinishPottery};
    return &productionType;
}

const SettlementProductionType *SettlementProductionFactory::BuildTailoryProduction()
{
    static const SettlementProductionType productionType = {ProductionOptions::TAILORY, "Tailory", 300, &ProductionFinisher::FinishTailory};
    return &productionType;
}

const SettlementProductionType *SettlementProductionFactory::BuildCarpenterProduction()
{
    static const SettlementProductionType productionType = {ProductionOptions::CARPENTER, "Carpenter", 300, &ProductionFinisher::FinishCarpenter};
    return &productionType;
}

const SettlementProductionType *SettlementProductionFactory::BuildBakeryProduction()
{
    static const SettlementProductionType productionType = {ProductionOptions::BAKERY, "Bakery", 300, &ProductionFinisher::FinishBakery};
    return &productionType;
}

const SettlementProductionType *SettlementProductionFactory::BuildMarketProduction()
{
    static const SettlementProductionType productionType = {ProductionOptions::MARKET, "Market", 300, &ProductionFinisher::FinishMarket};
    return &productionType;
}

const SettlementProductionType *SettlementProductionFactory::BuildKeepProduction()
{
    static const SettlementProductionType productionType = {ProductionOptions::KEEP, "Keep", 300, &ProductionFinisher::FinishKeep};
    return &productionType;
}

const SettlementProductionType * SettlementProductionFactory::BuildNoneProduction()
{
    static const SettlementProductionType productionType = {ProductionOptions::NONE, "Nothing", 6, nullptr};
    return &productionType;
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
    //auto target = settlement.currentProduction->data.improvementTarget;
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
    auto target = settlement.currentProduction->data.colonizationTarget;

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