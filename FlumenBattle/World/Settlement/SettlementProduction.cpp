#include "SettlementProduction.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Group/GroupDynamics.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/WorldScene.h"

using namespace world::settlement;

SettlementProduction SettlementProductionFactory::Create(SettlementProductionOptions option, ProductionData data)
{
    return {BuildProductionType(option), 0, data};
    /*switch(option)
    {
    case SettlementProductionOptions::FARM:
        return {BuildProductionType(option), 0, data};
    case SettlementProductionOptions::PATROL:
        return {BuildProductionType(option), 0, data};
    case SettlementProductionOptions::SETTLERS:
        return {BuildProductionType(option), 0, data};
    case SettlementProductionOptions::SEWAGE:
        return {BuildProductionType(option), 0, data};
    case SettlementProductionOptions::IRRIGATION:
        return {BuildProductionType(option), 0, data};
    case SettlementProductionOptions::NONE:
        return {BuildProductionType(option), 0, data};
    }*/
}

const SettlementProductionType * SettlementProductionFactory::BuildProductionType(SettlementProductionOptions option)
{
    switch(option)
    {
        case SettlementProductionOptions::PATROL:
            return BuildPatrolProduction();
        case SettlementProductionOptions::FARM:
            return BuildFarmProduction();
        case SettlementProductionOptions::SETTLERS:
            return BuildSettlersProduction();
        case SettlementProductionOptions::SEWAGE:
            return BuildSewageProduction();
        case SettlementProductionOptions::IRRIGATION:
            return BuildIrrigationProduction();
        case SettlementProductionOptions::LIBRARY:
            return BuildLibraryProduction();
        case SettlementProductionOptions::NONE:
            return BuildNoneProduction();
    }
}

const SettlementProductionType * SettlementProductionFactory::BuildPatrolProduction()
{
    static const SettlementProductionType productionType = {SettlementProductionOptions::PATROL, "Patrol", 100, &SettlementProductionFinisher::FinishPatrol};
    return &productionType;
}

const SettlementProductionType * SettlementProductionFactory::BuildFarmProduction()
{
    static const SettlementProductionType productionType = {SettlementProductionOptions::FARM, "Farm", 200, &SettlementProductionFinisher::FinishFarm};
    return &productionType;
}

const SettlementProductionType * SettlementProductionFactory::BuildSettlersProduction()
{
    static const SettlementProductionType productionType = {SettlementProductionOptions::SETTLERS, "Settlers", 100, &SettlementProductionFinisher::FinishSettlers};
    return &productionType;
}

const SettlementProductionType * SettlementProductionFactory::BuildSewageProduction()
{
    static const SettlementProductionType productionType = {SettlementProductionOptions::SEWAGE, "Sewage", 400, &SettlementProductionFinisher::FinishSewage};
    return &productionType;
}

const SettlementProductionType * SettlementProductionFactory::BuildIrrigationProduction()
{
    static const SettlementProductionType productionType = {SettlementProductionOptions::IRRIGATION, "Irrigation", 700, &SettlementProductionFinisher::FinishIrrigation};
    return &productionType;
}

const SettlementProductionType * SettlementProductionFactory::BuildLibraryProduction()
{
    static const SettlementProductionType productionType = {SettlementProductionOptions::LIBRARY, "Library", 500, &SettlementProductionFinisher::FinishLibrary};
    return &productionType;
}

const SettlementProductionType * SettlementProductionFactory::BuildNoneProduction()
{
    static const SettlementProductionType productionType = {SettlementProductionOptions::NONE, "None", 0, nullptr};
    return &productionType;
}

void SettlementProductionFinisher::FinishPatrol(Settlement &settlement)
{
    settlement.StrengthenPatrol();
}

void SettlementProductionFinisher::FinishSewage(Settlement &settlement)
{
    settlement.AddBuilding(BuildingTypes::SEWAGE);
}      

void SettlementProductionFinisher::FinishLibrary(Settlement &settlement)
{
    settlement.AddBuilding(BuildingTypes::LIBRARY);
}      

void SettlementProductionFinisher::FinishFarm(Settlement &settlement)
{
    auto target = settlement.currentProduction->data.improvementTarget;
    target->IsBuilt = true;
}      

void SettlementProductionFinisher::FinishSettlers(Settlement &settlement)
{
    auto polity = settlement.polity;
    auto target = settlement.currentProduction->data.colonizationTarget;

    if(target->IsBorderingOwnedTile() == true)
    {
        target = settlement.FindColonySpot();
    }

    if(target != nullptr)
    {
        auto dice = utility::GetRandom(1, 20);
        if(dice == 20)
        {
            WorldScene::Get()->FoundSettlement(target, polity, &settlement);
        }
        else if(dice > 5)
        {
            WorldScene::Get()->FoundSettlement(target, polity, &settlement);
            settlement.KillPopulation();
        }
        else if(dice > 1) //Colonization fails, but settlers survive & return
        {
            
        }
        else if(dice == 1) //Colonization fails and settlers perish
        {
            std::cout<<"critical failure\n";
            settlement.KillPopulation();
        }
    }
}

void SettlementProductionFinisher::FinishIrrigation(Settlement &settlement)
{
    settlement.AddBuilding(BuildingTypes::IRRIGATION);
}      