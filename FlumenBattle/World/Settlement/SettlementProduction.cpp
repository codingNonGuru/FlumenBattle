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
            return BuildSewageProduction();
        case SettlementProductionOptions::NONE:
            return BuildNoneProduction();
    }
}

const SettlementProductionType * SettlementProductionFactory::BuildPatrolProduction()
{
    static const SettlementProductionType productionType = {SettlementProductionOptions::PATROL, 100, &SettlementProductionFinisher::FinishPatrol};
    return &productionType;
}

const SettlementProductionType * SettlementProductionFactory::BuildFarmProduction()
{
    static const SettlementProductionType productionType = {SettlementProductionOptions::FARM, 200, &SettlementProductionFinisher::FinishFarm};
    return &productionType;
}

const SettlementProductionType * SettlementProductionFactory::BuildSettlersProduction()
{
    static const SettlementProductionType productionType = {SettlementProductionOptions::SETTLERS, 500, &SettlementProductionFinisher::FinishSettlers};
    return &productionType;
}

const SettlementProductionType * SettlementProductionFactory::BuildSewageProduction()
{
    static const SettlementProductionType productionType = {SettlementProductionOptions::SEWAGE, 400, &SettlementProductionFinisher::FinishSewage};
    return &productionType;
}

const SettlementProductionType * SettlementProductionFactory::BuildIrrigationProduction()
{
    static const SettlementProductionType productionType = {SettlementProductionOptions::SEWAGE, 700, &SettlementProductionFinisher::FinishIrrigation};
    return &productionType;
}

const SettlementProductionType * SettlementProductionFactory::BuildNoneProduction()
{
    static const SettlementProductionType productionType = {SettlementProductionOptions::NONE, 0, nullptr};
    return &productionType;
}

void SettlementProductionFinisher::FinishPatrol(Settlement &settlement)
{
    settlement.StrengthenPatrol();
}

void SettlementProductionFinisher::FinishSewage(Settlement &settlement)
{
    settlement.hasSewage = true;
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
            WorldScene::Get()->FoundSettlement(target, polity);
        }
        else if(dice > 5)
        {
            WorldScene::Get()->FoundSettlement(target, polity);
            settlement.population--;
        }
        else if(dice > 1) //Colonization fails, but settlers survive & return
        {
            
        }
        else if(dice == 1) //Colonization fails and settlers perish
        {
            std::cout<<"critical failure\n";
            settlement.population--;
        }
    }
}

void SettlementProductionFinisher::FinishIrrigation(Settlement &settlement)
{
    settlement.hasIrrigation = true;
}      