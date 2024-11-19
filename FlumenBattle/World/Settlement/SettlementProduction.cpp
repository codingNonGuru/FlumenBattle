#include <mutex>

#include "SettlementProduction.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Group/GroupDynamics.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Settlement/ProductionEvaluator.h"
#include "FlumenBattle/Race.h"
#include "FlumenBattle/Utility/Utility.h"

using namespace world::settlement;

#define COLONIZATION_DC 5

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
    case ProductionOptions::FARM:
    {
        SettlementTile *improvementTarget = nullptr;
        for(auto &tile : settlement.GetTiles())
        {
            if(tile.Tile->HasBiome(world::WorldBiomes::STEPPE) && tile.IsBuilt == false && tile.Tile != settlement.GetLocation())
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
        }
    }
    case ProductionOptions::SEWAGE:
        return {settlement.HasBuilding(BuildingTypes::SEWAGE) == false};
    case ProductionOptions::IRRIGATION:
        return {settlement.HasBuilding(BuildingTypes::IRRIGATION) == false};
    case ProductionOptions::LIBRARY:
        return {settlement.HasBuilding(BuildingTypes::LIBRARY) == false};
    case ProductionOptions::LUMBER_MILL:
        return {settlement.HasBuilding(BuildingTypes::LUMBER_MILL) == false};
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
        case ProductionOptions::NONE:
            return BuildNoneProduction();
    }
}

const SettlementProductionType * SettlementProductionFactory::BuildPatrolProduction()
{
    static const SettlementProductionType productionType = {ProductionOptions::PATROL, "Patrol", 200, &ProductionFinisher::FinishPatrol};
    return &productionType;
}

const SettlementProductionType * SettlementProductionFactory::BuildFarmProduction()
{
    static const SettlementProductionType productionType = {ProductionOptions::FARM, "Farm", 300, &ProductionFinisher::FinishFarm};
    return &productionType;
}

const SettlementProductionType * SettlementProductionFactory::BuildSettlersProduction()
{
    static const SettlementProductionType productionType = {ProductionOptions::SETTLERS, "Settlers", 500, &ProductionFinisher::FinishSettlers};
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

const SettlementProductionType * SettlementProductionFactory::BuildNoneProduction()
{
    static const SettlementProductionType productionType = {ProductionOptions::NONE, "Nothing", 6, nullptr};
    return &productionType;
}

void ProductionFinisher::FinishPatrol(Settlement &settlement)
{
    settlement.StrengthenPatrol();
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

void ProductionFinisher::FinishFarm(Settlement &settlement)
{
    auto target = settlement.currentProduction->data.improvementTarget;
    target->IsBuilt = true;
}      

void ProductionFinisher::FinishSettlers(Settlement &settlement)
{
    auto polity = settlement.polity;
    auto target = settlement.currentProduction->data.colonizationTarget;

    if(target->IsBorderingOwnedTile() == true)
    {
        target = settlement.FindColonySpot();
    }

    static std::mutex mutex;
    mutex.lock();

    if(target != nullptr)
    {
        auto bonus = settlement.GetModifier(Modifiers::ALL_DICE_ROLLS);

        auto check = utility::RollD20Dice(COLONIZATION_DC, bonus);
        if(check.IsCriticalSuccess() == true)
        {
            WorldScene::Get()->FoundSettlement(target, settlement.GetRace()->Type, &settlement);
        }
        else if(check.IsAnySuccess() == true)
        {
            WorldScene::Get()->FoundSettlement(target, settlement.GetRace()->Type, &settlement);
            settlement.KillPopulation();
        }
        else if(check.IsRegularFailure() == true) 
        {
            
        }
        else
        {
            settlement.KillPopulation();
        }
    }

    mutex.unlock();
}

void ProductionFinisher::FinishIrrigation(Settlement &settlement)
{
    settlement.AddBuilding(BuildingTypes::IRRIGATION);
}      

void ProductionFinisher::FinishHousing(Settlement &settlement)
{
    settlement.AddBuilding(BuildingTypes::HOUSING);
}      