#include "PopHandler.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Polity/Polity.h"
#include "FlumenBattle/Config.h"
#include "FlumenBattle/Utility/Utility.h"
#include "FlumenBattle/World/WorldTime.h"
#include "FlumenBattle/World/Settlement/PopExtraData.h"
#include "FlumenBattle/World/Settlement/Cohort.h"

using namespace world::settlement;

static const auto HAPPINESS_DETERMINANT_COUNT = 4;

static const auto RECOVERY_FROM_ABANDONMENT_THRESHOLD = -5;

static const auto FALL_INTO_RUINS_THRESHOLD = 5;

static const auto ABANDONMENT_RECOVERY_DC = 15;

static const auto COMPLETE_DISAPPEARENCE_TIME = world::WorldTime::GetTicksFromDays(7);

static const auto GROWTH_THRESHOLD = 10000;

void PopHandler::Initialize(Settlement *settlement)
{
    this->settlement = settlement;

    population = 5;

    highestPopulationEver = population;

    timeSinceAbandonment = 0;

    abandonmentSeverity = 0;

    growth = 0;

    needs.Reset();

    *needs.Add() = {ResourceTypes::FOOD, false, true, 0};
    *needs.Add() = {ResourceTypes::COOKED_FOOD, true, true, 0};
    *needs.Add() = {ResourceTypes::FURNITURE, true, false, 80};
    *needs.Add() = {ResourceTypes::CLOTHING, true, false, 80};
    *needs.Add() = {ResourceTypes::POTTERY, true, false, 80};

    happiness = 0;

    isHappy = false;

    extraData = nullptr;
}

void PopHandler::Deepen()
{
    extraData->Initialize(this);
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

    if(GetPopulation() == 0)
    {
        return;
    }

    auto consumption = GetPopulation() * cookedFood->Type->PopulationConsumption;
    if(consumption <= cookedFood->Storage)
    {
        cookedFood->Order += consumption;

        cookedFood->HasPopulationOrdered = true;
        rawFood->HasPopulationOrdered = false;
    }
    else
    {
        consumption = GetPopulation() * rawFood->Type->PopulationConsumption;

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

    consumption = GetPopulation() * clothing->Type->PopulationConsumption;
    if(consumption <= clothing->Storage && needs.Find(ResourceTypes::CLOTHING)->Satisfaction < needs.Find(ResourceTypes::CLOTHING)->SatisfactionThreshold)
    {
        clothing->Order += consumption;

        clothing->HasPopulationOrdered = true;
    }

    consumption = GetPopulation() * pottery->Type->PopulationConsumption;
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

        needs.Find(ResourceTypes::CLOTHING)->Satisfaction += 15;
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

int PopHandler::GetPopulation() const
{
    return population;
}

float PopHandler::GetGrowthRatio() const 
{
    return float(growth) / float(GROWTH_THRESHOLD);
}

bool PopHandler::IsSettlementAbandoned() const
{
    return GetPopulation() == 0;
}

int PopHandler::GetAbandonmentSeverity() const
{
    return abandonmentSeverity;
}

bool PopHandler::IsSettlementRuins() const
{
    return IsSettlementAbandoned() && abandonmentSeverity == FALL_INTO_RUINS_THRESHOLD;
}

bool PopHandler::IsSettlementCompletelyGone() const
{
    return IsSettlementRuins() && timeSinceRuined >= COMPLETE_DISAPPEARENCE_TIME;
}

const container::Pool <Cohort> &PopHandler::GetCohorts() const
{
    static const container::Pool <Cohort> dummyCohorts;

    return extraData != nullptr ? extraData->GetCohorts() : dummyCohorts;
}

void PopHandler::IncreasePopulation(Settlement *settlement)
{
    population++;

    if(population > highestPopulationEver)
    {
        highestPopulationEver = population;
    }

    settlement->GetPolity()->RegisterPopIncrease(settlement);
}

void PopHandler::KillPopulation()
{
    population--;
    if(population < 0)
    {
        population = 0;
    }
}

void PopHandler::CheckAbandonment(Settlement *settlement)
{
    if(settlement->IsValid() == false)
        return;

    if(IsSettlementAbandoned() == true)
    {
        timeSinceAbandonment++;

        if(IsSettlementRuins() == false)
        {
            if(timeSinceAbandonment != 0 && timeSinceAbandonment % 144 == 0)
            {
                auto savingThrow = utility::RollD20Dice(ABANDONMENT_RECOVERY_DC);

                if(savingThrow.IsCriticalSuccess() == true)
                {
                    abandonmentSeverity -= 2;
                    if(abandonmentSeverity <= RECOVERY_FROM_ABANDONMENT_THRESHOLD)
                    {
                        abandonmentSeverity = RECOVERY_FROM_ABANDONMENT_THRESHOLD;

                        IncreasePopulation(settlement);
                    }
                }
                else if(savingThrow.IsNormalSuccess() == true)
                {
                    abandonmentSeverity--;
                    if(abandonmentSeverity == RECOVERY_FROM_ABANDONMENT_THRESHOLD)
                    {
                        IncreasePopulation(settlement);
                    }
                }
                else if(savingThrow.IsRegularFailure() == true)
                {
                    abandonmentSeverity++;
                }
                else
                {
                    abandonmentSeverity += 2;
                    if(abandonmentSeverity > FALL_INTO_RUINS_THRESHOLD)
                    {
                        abandonmentSeverity = FALL_INTO_RUINS_THRESHOLD;
                    }
                }

                if(abandonmentSeverity == FALL_INTO_RUINS_THRESHOLD)
                {
                    timeSinceRuined = 0;
                }
            }
        }
        else
        {
            timeSinceRuined++;
        }
    }
    else
    {
        timeSinceAbandonment = 0;

        abandonmentSeverity = 0;
    }
}

void PopHandler::UpdateGrowth(Settlement *settlement)
{
    auto foodSecurity = settlement->GetResource(ResourceTypes::FOOD)->ShortTermAbundance;
    auto housingAdequacy = settlement->GetHousingAdequacy();

    auto addedGrowth = [&]
    {
        auto growthFromFood = [&]
        {
            switch(foodSecurity)
            {
            case AbundanceLevels::CORNUCOPIA:
                return 5;
            case AbundanceLevels::ABUNDANT:
                return 4;
            case AbundanceLevels::ENOUGH:
                return 3;
            case AbundanceLevels::BARELY_AVAILABLE:
                return 0;
            case AbundanceLevels::LACKING:
                return -5;
            case AbundanceLevels::SORELY_LACKING:
                return -7;
            }
        } ();

        auto growthFromHousing = [&]
        {
            switch(housingAdequacy)
            {
            case AbundanceLevels::ENOUGH:
                return 1;
            case AbundanceLevels::BARELY_AVAILABLE:
                return 0;
            case AbundanceLevels::LACKING:
                return -1;
            case AbundanceLevels::SORELY_LACKING:
                return -3;
            }
        } ();

        auto modifier = settlement->GetModifier(Modifiers::POPULATION_GROWTH_RATE);

        return growthFromFood + growthFromHousing + modifier;
    } ();

    growth += addedGrowth;
    if(growth >= GROWTH_THRESHOLD)
    {
        growth = 0;
        
        IncreasePopulation(settlement);

        settlement->PromptWorkReorganizing();
    }
    else if(growth < 0)
    {
        growth = 0;
    }
}

void PopAllocator::AllocateExtraData(container::Pool <PopExtraData> &extraDataAllocator, container::PoolAllocator <Cohort> &cohortAllocator, PopHandler &handler) 
{
    if(handler.extraData != nullptr)
        return;

    handler.extraData = extraDataAllocator.Add();

    handler.extraData->cohorts.Initialize(cohortAllocator);
}

void PopAllocator::FreeExtraData(container::Pool <PopExtraData> &extraDataAllocator, container::PoolAllocator <Cohort> &cohortAllocator, PopHandler &handler)
{
    if(handler.extraData == nullptr)
        return;

    handler.extraData->cohorts.Terminate(cohortAllocator);

    extraDataAllocator.RemoveAt(handler.extraData);

    handler.extraData = nullptr;
}