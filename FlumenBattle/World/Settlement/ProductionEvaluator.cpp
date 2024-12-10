#include "ProductionEvaluator.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Group/GroupDynamics.h"
#include "FlumenBattle/Config.h"

using namespace world::settlement;

NecessityFactor ProductionEvaluator::GetPatrolNecessity(const Settlement &settlement)
{
    auto &groupDynamics = settlement.GetGroupDynamics();
    auto patrolDeficiency = groupDynamics.GetBanditStrength() - groupDynamics.GetPatrolStrength();
    if(patrolDeficiency >= 7)
    {
        return 9;
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

NecessityFactor ProductionEvaluator::GetGarrisonNecessity(const Settlement &settlement)
{
    auto &groupDynamics = settlement.GetGroupDynamics();
    auto garrisonStrength = groupDynamics.GetGarrisonStrength();
    if(garrisonStrength == 0)
    {
        return 10;
    }
    else
    {
        return 0;
    }
}

NecessityFactor ProductionEvaluator::GetSewageNecessity(const Settlement &settlement)
{
    if(settlement.HasBuilding(BuildingTypes::IRRIGATION) == true)
        return 0;

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
        return 8;
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
        return 1;
    }
}

NecessityFactor ProductionEvaluator::GetFarmNecessity(const Settlement &settlement)
{
    auto foodSecurity = settlement.GetResource(ResourceTypes::FOOD)->LongTermAbundance;
    switch(foodSecurity)
    {
    case AbundanceLevels::SORELY_LACKING:
        return 10;
    case AbundanceLevels::LACKING:
        return 8;
    case AbundanceLevels::BARELY_AVAILABLE:
        return 7;
    case AbundanceLevels::ENOUGH:
        return 5;
    case AbundanceLevels::ABUNDANT:
        return 2;
    case AbundanceLevels::CORNUCOPIA:
        return 0;
    }
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

NecessityFactor ProductionEvaluator::GetWallsNecessity(const Settlement &settlement)
{
    static const auto MAXIMUM_WALLS_LEVEL = engine::ConfigManager::Get()->GetValue(game::ConfigValues::MAXIMUM_WALLS_LEVEL).Integer;

    if(settlement.GetBuildingCount(BuildingTypes::WALLS) < MAXIMUM_WALLS_LEVEL)
        return 1;
    else
        return 0;
}

NecessityFactor ProductionEvaluator::GetHousingNecessity(const Settlement &settlement)
{
    auto housingAdequacy = settlement.GetHousingAdequacy();

    switch(housingAdequacy)
    {
    case AbundanceLevels::ENOUGH:
        return 0;
    case AbundanceLevels::BARELY_AVAILABLE:
        return 2;
    case AbundanceLevels::LACKING:
        return 5;
    case AbundanceLevels::SORELY_LACKING:
        return 10;
    }
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
    case ProductionOptions::GARRISON:
        return GetGarrisonNecessity(settlement);
    case ProductionOptions::SEWAGE:
        return GetSewageNecessity(settlement);
    case ProductionOptions::HOUSING:
        return GetHousingNecessity(settlement);
    case ProductionOptions::WALLS:
        return GetWallsNecessity(settlement);
    default:
        abort();
    }
}