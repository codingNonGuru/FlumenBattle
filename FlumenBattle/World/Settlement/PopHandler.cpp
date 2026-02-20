#include "PopHandler.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/SettlementTile.h"
#include "FlumenBattle/World/Polity/Polity.h"
#include "FlumenBattle/Config.h"
#include "FlumenBattle/Utility/Utility.h"
#include "FlumenBattle/World/WorldTime.h"
#include "FlumenBattle/World/Settlement/PopExtraData.h"
#include "FlumenBattle/World/Settlement/Cohort.h"
#include "FlumenBattle/World/Tile/WorldTile.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/RaceFactory.h"

using namespace world::settlement;

static const auto HAPPINESS_DETERMINANT_COUNT = 4;

static const auto RECOVERY_FROM_ABANDONMENT_THRESHOLD = -5;

static const auto FALL_INTO_RUINS_THRESHOLD = 5;

static const auto ABANDONMENT_RECOVERY_DC = 15;

static const auto COMPLETE_DISAPPEARENCE_TIME = world::WorldTime::GetTicksFromDays(7);

static const auto GROWTH_THRESHOLD = 10000;

static const auto POP_REGENERATION_RATE = 1;

const container::Array <RaceGroup> &PopHandler::GetNeighbourRaces()
{
    static container::Array <RaceGroup> neighbourRaces(64);

    neighbourRaces.Reset();

    for(auto &tile : settlement->GetTiles())
    {
        auto group = neighbourRaces.Find(tile.Tile->MajorRace);
        if(group != nullptr)
        {
            group->Size += 2;
        }
        else
        {
            *neighbourRaces.Add() = {tile.Tile->MajorRace, 2};
        }

        group = neighbourRaces.Find(tile.Tile->MinorRace);
        if(group != nullptr)
        {
            group->Size += 1;
        }
        else
        {
            *neighbourRaces.Add() = {tile.Tile->MinorRace, 1};
        }
    }

    return neighbourRaces;
}

void PopHandler::Initialize(Settlement *settlement)
{
    this->settlement = settlement;

    population = 0;

    highestPopulationEver = 0;

    auto &races = GetNeighbourRaces();
    for(int i = 0; i < 12; ++i)
    {
        IncreasePopulation(settlement, &races);
    }

    timeSinceAbandonment = 0;

    abandonmentSeverity = 0;

    growth = 0;

    currentHealth = population * Cohort::MAXIMUM_HEALTH;

    needs.Reset();

    *needs.Add() = {ResourceTypes::FOOD, false, true, 500, 15};
    *needs.Add() = {ResourceTypes::COOKED_FOOD, true, true, 500, 15};
    *needs.Add() = {ResourceTypes::FURNITURE, true, false, 800, 200};
    *needs.Add() = {ResourceTypes::CLOTHING, true, false, 800, 150};
    *needs.Add() = {ResourceTypes::POTTERY, true, false, 800, 100};

    happiness = 0;

    isHappy = false;

    extraData = nullptr;
}

void PopHandler::Deepen()
{
    extraData->Initialize(this);
}

auto needHierarchies = {container::Array <ResourceTypes> {ResourceTypes::COOKED_FOOD, ResourceTypes::FOOD}};

void PopHandler::PlaceOrders(Settlement &settlement)
{
    for(auto &need : needs)
    {
        settlement.GetResource(need.Type)->HasPopulationOrdered = false;

        need.IsMet = false;

        //need.Order = 0;
    }

    if(GetPopulation() == 0)
        return;

    for(auto &need : needs)
    {
        auto resource = settlement.GetResource(need.Type);
        auto consumption = GetPopulation() * resource->Type->PopulationConsumption;

        if(consumption <= resource->Storage && need.Satisfaction < need.SatisfactionThreshold)
        {
            need.Order = consumption;

            resource->HasPopulationOrdered = true;
        }
    }


    for(auto &hierarchy : needHierarchies)
    {
        bool isOneNeedFulfilled = false;
        for(auto &resourceType : hierarchy)
        {
            auto resource = settlement.GetResource(resourceType);
            if(isOneNeedFulfilled == true)
            {
                resource->HasPopulationOrdered = false;
                continue;
            }

            //auto need = needs.Find(resourceType);
            if(resource->HasPopulationOrdered == true)
            {
                isOneNeedFulfilled = true;
            }
        }
    }

    for(auto &need : needs)
    {
        auto resource = settlement.GetResource(need.Type);
        if(resource->HasPopulationOrdered == false)
            continue;

        resource->Order += need.Order;
    }
}

void PopHandler::UpdateNeeds(Settlement &settlement)
{
    for(auto &need : needs)
    {
        need.IsMet = false;
    }

    for(auto &need : needs)
    {
        auto resource = settlement.GetResource(need.Type);
        if(resource->HasPopulationOrdered == false)
            continue;

        need.IsMet = true;

        need.Satisfaction += need.SatisfactionBoostPerConsumption;
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

const Race *PopHandler::GetMostPopulousRace()
{
    auto largestGroup = raceGroups.GetStart();
    for(auto &group : raceGroups)
    {
        if(group.Size > largestGroup->Size)
        {
            largestGroup = &group;
        }
    }

    return RaceFactory::BuildRace(largestGroup->Race);
}

int PopHandler::GetPotentialMidtermConsumption(ResourceTypes resource) const
{
    static const auto &time = WorldScene::Get()->GetTime();
    static const auto ticks = time.GetTicksFromDays(3);

    auto need = needs.Find(resource);
    if(need == nullptr)
        return 0;

    auto consumption = (ticks * ResourceFactory::Get()->CreateType(resource)->PopulationConsumption * population) / need->SatisfactionBoostPerConsumption;

    return consumption;
}

void PopHandler::IncreasePopulation(Settlement *settlement, const container::Array <RaceGroup> *races)
{
    if(races == nullptr)
        races = &GetNeighbourRaces();

    auto chosenGroup = utility::GetWeightedRandom(*races);

    auto existingGroup = raceGroups.Find(chosenGroup->Race);
    if(existingGroup == nullptr)
    {
        *raceGroups.Add() = {chosenGroup->Race, 1};
    }
    else
    {
        existingGroup->Size += 1;
    }

    population++;

    if(population > highestPopulationEver)
    {
        highestPopulationEver = population;
    }

    if(extraData != nullptr)
    {
        extraData->AddPopulation(chosenGroup->Race);
    }

    if(settlement->GetPolity() != nullptr)
    {
        settlement->GetPolity()->RegisterPopIncrease(settlement);
    }
}

void PopHandler::KillPopulation()
{
    population--;
    if(population < 0)
    {
        population = 0;
    }

    if(extraData != nullptr)
    {
        auto race = extraData->KillRandomPopulation();

        auto group = raceGroups.Find(race);
        group->Size--;

        if(group->Size == 0)
        {
            //raceGroups
        }
    }
    else
    {
        auto group = raceGroups.GetRandom();

        group->Size--;

        if(group->Size == 0)
        {
            
        }
    }

    if(settlement->GetPolity() != nullptr)
    {
        settlement->GetPolity()->RegisterPopDecrease(settlement);
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

    RegenerateHealth();
}

void PopHandler::UpdatePops()
{
    for(auto &cohort : GetCohorts())
    {
        cohort.Update();
    }
}

void PopHandler::RegenerateHealth()
{
    static const auto &worldTime = world::WorldScene::Get()->GetTime();
    if(worldTime.IsNewDayQuarter == false)
        return;

    if(extraData == nullptr)
    {
        currentHealth += population * POP_REGENERATION_RATE;
        if(currentHealth > population * Cohort::MAXIMUM_HEALTH)
        {
            currentHealth = population * Cohort::MAXIMUM_HEALTH;
        }
    }
    else
    {
        currentHealth = 0;

        for(auto &cohort : extraData->GetCohorts())
        {
            cohort.Health += POP_REGENERATION_RATE;
            if(cohort.Health > Cohort::MAXIMUM_HEALTH)
            {
                cohort.Health = Cohort::MAXIMUM_HEALTH;
            }

            currentHealth += cohort.Health;
        }
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