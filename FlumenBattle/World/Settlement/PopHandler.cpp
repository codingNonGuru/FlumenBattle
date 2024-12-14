#include "PopHandler.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/Config.h"

using namespace world::settlement;

void PopHandler::Initialize()
{
    needs.Reset();

    *needs.Add() = {ResourceTypes::FOOD};
    *needs.Add() = {ResourceTypes::COOKED_FOOD};
    *needs.Add() = {ResourceTypes::FURNITURE};
}

void PopHandler::PlaceOrders(Settlement &settlement)
{
    auto cookedFood = settlement.GetResource(ResourceTypes::COOKED_FOOD);
    auto rawFood = settlement.GetResource(ResourceTypes::FOOD);

    auto consumption = settlement.GetPopulation() * cookedFood->Type->PopulationConsumption;
    if(consumption <= cookedFood->Storage)
    {
        cookedFood->Order += consumption;

        cookedFood->HasPopulationOrdered = true;
        rawFood->HasPopulationOrdered = false;
    }
    else
    {
        consumption = settlement.GetPopulation() * rawFood->Type->PopulationConsumption;

        if(consumption <= rawFood->Storage)
        {
            rawFood->Order += consumption;

            cookedFood->HasPopulationOrdered = false;
            rawFood->HasPopulationOrdered = true;
        }
        else
        {
            cookedFood->HasPopulationOrdered = false;
            rawFood->HasPopulationOrdered = false;
        }
    }
}

void PopHandler::UpdateNeeds(Settlement &settlement)
{
    for(auto &need : needs)
    {
        need.IsMet = false;
    }

    auto cookedFood = settlement.GetResource(ResourceTypes::COOKED_FOOD);
    auto rawFood = settlement.GetResource(ResourceTypes::FOOD);

    if(cookedFood->HasPopulationOrdered == true)
    {
        needs.Find(ResourceTypes::COOKED_FOOD)->IsMet = true;

        needs.Find(ResourceTypes::COOKED_FOOD)->Satisfaction += 1;
    }
    else if(rawFood->HasPopulationOrdered == true)
    {
        needs.Find(ResourceTypes::FOOD)->IsMet = true;

        needs.Find(ResourceTypes::FOOD)->Satisfaction += 1;
    }

    static const auto TICKS_PER_NEED_SATISFACTION = engine::ConfigManager::Get()->GetValue(game::ConfigValues::TICKS_PER_NEED_SATISFACTION).Integer;

    for(auto &need : needs)
    {
        if(need.IsMet == false)
        {
            need.Satisfaction -=1;
        }

        if(need.Satisfaction > TICKS_PER_NEED_SATISFACTION)
        {
            need.Satisfaction = TICKS_PER_NEED_SATISFACTION;
        }

        if(need.Satisfaction < 0)
        {
            need.Satisfaction = 0;
        }
    }
}