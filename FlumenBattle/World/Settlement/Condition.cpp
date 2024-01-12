#include "Condition.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/Modifier.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Types.h"

using namespace world::settlement;

void ConditionManager::AddCondition(ConditionData conditionData)
{
    auto result = conditionSet.Get().Find(conditionData.Type);
    if(result != nullptr)
    {
        result->Duration += conditionData.Duration;
        if(conditionData.Strength > result->Strength)
        {
            result->Strength = conditionData.Strength;
        }
    }
    else
    {
        *conditionSet.Get().Add() = ConditionFactory::Get()->Create(conditionData);
    }
}

void ConditionManager::ApplyModifiers(Settlement &settlement)
{
    for(auto &condition : conditionSet.Get())
    {
        condition.Apply(settlement);
    }
}

void ConditionManager::Update()
{
    auto &time = WorldScene::Get()->GetTime();
    for(auto &condition : conditionSet.Get())
    {
        if(time.MinuteCount == 0)
        {
            condition.HoursElapsed++;

            if(condition.HoursElapsed >= condition.Duration)
            {
                conditionSet.Get().RemoveAt(&condition);
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
    }
}

const ConditionType &ConditionFactory::BuildMalariaImmunity()
{
    static const ConditionType &type = {Conditions::IMMUNITY_AGAINST_MALARIA, &ConditionFactory::OnApplyMalariaImmunity};
    return type;
}

const ConditionType &ConditionFactory::BuildSickened()
{
    static const ConditionType &type = {Conditions::SICKENED, &ConditionFactory::OnApplySickened};
    return type;
}

const ConditionType &ConditionFactory::BuildRepressed()
{
    static const ConditionType &type = {Conditions::REPRESSED, &ConditionFactory::OnApplyRepressed};
    return type;
}