#include "PopHandler.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/Config.h"

using namespace world::settlement;

static const auto HAPPINESS_DETERMINANT_COUNT = 4;

void PopHandler::Initialize()
{
    needs.Reset();

    *needs.Add() = {ResourceTypes::FOOD, false, true, 0};
    *needs.Add() = {ResourceTypes::COOKED_FOOD, true, true, 0};
    *needs.Add() = {ResourceTypes::FURNITURE, true, false, 90};
    *needs.Add() = {ResourceTypes::CLOTHING, true, false, 90};
    *needs.Add() = {ResourceTypes::POTTERY, true, false, 90};

    happiness = 0;

    isHappy = false;
}

void PopHandler::PlaceOrders(Settlement &settlement)
{
    auto cookedFood = settlement.GetResource(ResourceTypes::COOKED_FOOD);
    auto rawFood = settlement.GetResource(ResourceTypes::FOOD);
    auto clothing = settlement.GetResource(ResourceTypes::CLOTHING);
    auto pottery = settlement.GetResource(ResourceTypes::POTTERY);

    for(auto &need : needs)
    {
        settlement.GetResource(need.Type)->HasPopulationOrdered = false;
    }

    if(settlement.GetPopulation() == 0)
    {
        return;
    }

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

    consumption = settlement.GetPopulation() * clothing->Type->PopulationConsumption;
    if(consumption <= clothing->Storage && needs.Find(ResourceTypes::CLOTHING)->Satisfaction < needs.Find(ResourceTypes::CLOTHING)->SatisfactionThreshold)
    {
        clothing->Order += consumption;

        clothing->HasPopulationOrdered = true;
    }

    consumption = settlement.GetPopulation() * pottery->Type->PopulationConsumption;
    if(consumption <= pottery->Storage && needs.Find(ResourceTypes::POTTERY)->Satisfaction < needs.Find(ResourceTypes::POTTERY)->SatisfactionThreshold)
    {
        pottery->Order += consumption;

        pottery->HasPopulationOrdered = true;
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
    auto clothing = settlement.GetResource(ResourceTypes::CLOTHING);
    auto pottery = settlement.GetResource(ResourceTypes::POTTERY);

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

    if(clothing->HasPopulationOrdered == true)
    {
        needs.Find(ResourceTypes::CLOTHING)->IsMet = true;

        needs.Find(ResourceTypes::CLOTHING)->Satisfaction += 7;
    }

    if(pottery->HasPopulationOrdered == true)
    {
        needs.Find(ResourceTypes::POTTERY)->IsMet = true;

        needs.Find(ResourceTypes::POTTERY)->Satisfaction += 10;
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

    static const auto NEED_SATISFACTION_THRESHOLD = engine::ConfigManager::Get()->GetValue(game::ConfigValues::NEED_SATISFACTION_THRESHOLD).Integer;
    static const auto TICKS_PER_HAPPINESS = engine::ConfigManager::Get()->GetValue(game::ConfigValues::TICKS_PER_HAPPINESS).Integer;

    happiness = 0;
    for(auto &need : needs)
    {
        if(need.DoesDetermineHappiness == false)
            continue;

        if(need.Satisfaction > NEED_SATISFACTION_THRESHOLD)
        {
            need.GeneratedHappiness += 1;
        }
        else
        {
            need.GeneratedHappiness -= 1;
        }

        if(need.GeneratedHappiness > TICKS_PER_HAPPINESS)
        {
            need.GeneratedHappiness = TICKS_PER_HAPPINESS;
        }
        else if(need.GeneratedHappiness < 0)
        {
            need.GeneratedHappiness = 0;
        }        

        happiness += need.GeneratedHappiness;
    }

    static const auto CONTENTEDNESS_THRESHOLD = engine::ConfigManager::Get()->GetValue(game::ConfigValues::CONTENTEDNESS_THRESHOLD).Integer;
    isContent = happiness > CONTENTEDNESS_THRESHOLD * HAPPINESS_DETERMINANT_COUNT;

    static const auto HAPPINESS_THRESHOLD = engine::ConfigManager::Get()->GetValue(game::ConfigValues::HAPPINESS_THRESHOLD).Integer;
    isHappy = happiness > HAPPINESS_THRESHOLD * HAPPINESS_DETERMINANT_COUNT;

    static const auto ECSTASY_THRESHOLD = engine::ConfigManager::Get()->GetValue(game::ConfigValues::ECSTASY_THRESHOLD).Integer;
    isEcstatic = happiness > ECSTASY_THRESHOLD * HAPPINESS_DETERMINANT_COUNT;
}

float PopHandler::GetHappinessRatio() const
{
    static const auto TICKS_PER_HAPPINESS = engine::ConfigManager::Get()->GetValue(game::ConfigValues::TICKS_PER_HAPPINESS).Integer;

    auto maximum = TICKS_PER_HAPPINESS * HAPPINESS_DETERMINANT_COUNT;

    return (float)happiness / (float)maximum;
}
