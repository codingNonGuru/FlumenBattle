#include "Condition.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/Modifier.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Types.h"

using namespace world::settlement;

void ConditionManager::AddCondition(ConditionData conditionData)
{
    auto result = conditionSet.conditions.Find(conditionData.Type);
    if(result != nullptr)
    {
        result->Duration = conditionData.Duration;
        if(conditionData.Strength > result->Strength)
        {
            result->Strength = conditionData.Strength;
        }

        result->HoursElapsed = 0;
    }
    else
    {
        *conditionSet.conditions.Add() = ConditionFactory::Get()->Create(conditionData);
    }
}

void ConditionManager::RemoveCondition(Conditions condition)
{
    conditionSet.conditions.Remove(condition);
}

void ConditionManager::ApplyModifiers(Settlement &settlement)
{
    for(auto &condition : conditionSet.conditions)
    {
        condition.Apply(settlement);
    }
}

void ConditionManager::Update()
{
    static const auto &time = WorldScene::Get()->GetTime();
    
    for(auto &condition : conditionSet.conditions)
    {
        if(condition.Type->IsDurationBased == false)
            continue;

        if(time.MinuteCount == 0)
        {
            condition.HoursElapsed++;

            if(condition.HoursElapsed >= condition.Duration)
            {
                condition.Type = nullptr;
                conditionSet.conditions.RemoveAt(&condition);
            }
        }
    }
}

void ConditionFactory::OnApplyMalariaImmunity(Settlement &settlement)
{
    settlement.AddModifier({Modifiers::MALARIA_EMERGENCE_DIFFICULTY, 1});
    settlement.AddModifier({Modifiers::SAVING_THROWS_AGAINST_MALARIA, 1});
}

void ConditionFactory::OnApplySickened(Settlement &settlement)
{
    settlement.AddModifier({Modifiers::ALL_DICE_ROLLS, -1});
}

void ConditionFactory::OnApplyRepressed(Settlement &settlement)
{
    settlement.AddModifier({Modifiers::FACTION_JOIN_INTERDICTION, 1});
}

void ConditionFactory::OnApplyHappiness(Settlement &settlement)
{
    settlement.AddModifier({Modifiers::ALL_DICE_ROLLS, 1});
}

void ConditionFactory::OnApplyHunger(Settlement &settlement)
{
    
}

Condition ConditionFactory::Create(ConditionData conditionData)
{
    auto &type = BuildType(conditionData.Type);
    return {&type, conditionData.Strength, conditionData.Duration, 0};
}

const ConditionType &ConditionFactory::BuildType(Conditions condition)
{
    switch(condition)
    {
    case Conditions::IMMUNITY_AGAINST_MALARIA:
        return BuildMalariaImmunity();
    case Conditions::SICKENED:
        return BuildSickened();
    case Conditions::REPRESSED:
        return BuildRepressed();
    case Conditions::HAPPINESS:
        return BuildHappiness();
    case Conditions::HUNGER:
        return BuildHunger();
    }
}

const ConditionType &ConditionFactory::BuildMalariaImmunity()
{
    static const ConditionType &type = {Conditions::IMMUNITY_AGAINST_MALARIA, "Immunity", "ConditionImmunity", true, &ConditionFactory::OnApplyMalariaImmunity};
    return type;
}

const ConditionType &ConditionFactory::BuildSickened()
{
    static const ConditionType &type = {Conditions::SICKENED, "Sickened", "ConditionSickness", false, &ConditionFactory::OnApplySickened};
    return type;
}

const ConditionType &ConditionFactory::BuildRepressed()
{
    static const ConditionType &type = {Conditions::REPRESSED, "Repressed", "ConditionRepressed", true, &ConditionFactory::OnApplyRepressed};
    return type;
}

const ConditionType &ConditionFactory::BuildHappiness()
{
    static const ConditionType &type = {Conditions::HAPPINESS, "Happiness", "ConditionHappiness", false, &ConditionFactory::OnApplyHappiness};
    return type;
}

const ConditionType &ConditionFactory::BuildHunger()
{
    static const ConditionType &type = {Conditions::HUNGER, "Hunger", "ConditionHunger", false, &ConditionFactory::OnApplyHunger};
    return type;
}