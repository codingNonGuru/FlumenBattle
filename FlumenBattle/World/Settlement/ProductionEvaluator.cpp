#include "ProductionEvaluator.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Group/GroupDynamics.h"

using namespace world::settlement;

NecessityFactor ProductionEvaluator::GetPatrolNecessity(const Settlement &settlement)
{
    auto &groupDynamics = settlement.GetGroupDynamics();
    auto patrolDeficiency = groupDynamics.GetBanditStrength() - groupDynamics.GetPatrolStrength();
    if(patrolDeficiency >= 7)
    {
        return 10;
    }
    else if(patrolDeficiency >= 3)
    {
        return 5;
    }
    else if(patrolDeficiency >= 1)
    {
        return 2;
    }
    else
    {
        return 0;
    }
}     

NecessityFactor ProductionEvaluator::GetSewageNecessity(const Settlement &settlement)
{
    auto population = settlement.GetPopulation();

    if(population >= 10)
    {
        return 3;
    }
    else if(population >= 5)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

NecessityFactor ProductionEvaluator::GetSettlersNecessity(const Settlement &settlement)
{
    if(settlement.HasAvailableColonySpots() == false)
        return 0;

    auto population = settlement.GetPopulation();

    if(population >= 30)
    {
        return 10;
    }
    else if(population >= 15)
    {
        return 5;
    }
    else if(population >= 8)
    {
        return 2;
    }
    else if(population >= 5)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

NecessityFactor ProductionEvaluator::GetFarmNecessity(const Settlement &settlement)
{
    return 0;
}

NecessityFactor ProductionEvaluator::GetIrrigationNecessity(const Settlement &settlement)
{
    if(settlement.HasBuilding(BuildingTypes::IRRIGATION) == false)
        return 1;
    else
        return 0;
} 

NecessityFactor ProductionEvaluator::GetLibraryNecessity(const Settlement &settlement)
{
    if(settlement.HasBuilding(BuildingTypes::LIBRARY) == false)
        return 1;
    else
        return 0;
}

NecessityFactor ProductionEvaluator::GetNecessity(const Settlement &settlement, ProductionOptions option)
{
    switch(option)
    {
    case ProductionOptions::FARM:
        return GetFarmNecessity(settlement);
    case ProductionOptions::LIBRARY:
        return GetLibraryNecessity(settlement);
    case ProductionOptions::IRRIGATION:
        return GetIrrigationNecessity(settlement);
    case ProductionOptions::SETTLERS:
        return GetSettlersNecessity(settlement);
    case ProductionOptions::PATROL:
        return GetPatrolNecessity(settlement);
    case ProductionOptions::SEWAGE:
        return GetSewageNecessity(settlement);
    default:
        abort();
    }
}