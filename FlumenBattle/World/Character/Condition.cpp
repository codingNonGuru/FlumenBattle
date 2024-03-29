/*#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenBattle/World/Character/Condition.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/Battle/Combatant.h"

using namespace world::character;

Condition::Condition() {duration = 0;}

Condition::Condition(ConditionTypes _type, battle::Combatant * _combatant, Integer _difficultyClass) : combatant(_combatant)
{
    type = _type;
    difficultyClass = _difficultyClass;

    switch(type)
    {
        case ConditionTypes::CHILL:
            InitializeCold();
            break;
        case ConditionTypes::EVASION:
            InitializeEvasion();
            break;
        default:
            break;
    }
}

void Condition::Apply()
{
    if(IsActive() == false)
        return;

    duration--;
    if(duration == 0)
    {
        combatant->conditions.Remove(this);
    }

    switch(type)
    {
        case ConditionTypes::CHILL:
            ApplyCold();
            break;
        case ConditionTypes::EVASION:
            ApplyEvasion();
            break;
        default:
            break;
    }
}

void Condition::InitializeCold() 
{
    duration = 3;
}

void Condition::ApplyCold() 
{   
    auto roll = utility::GetRandom(1, 20);
    if(roll == 20)
    {
        if(duration != 0)
        {
            combatant->conditions.Remove(this);
            duration = 0;
        }
        return;
    }
    else if(roll == 1)
    {
        combatant->speedPenalty += 4;
    }
    else
    {
        auto savingThrow = roll + combatant->character->GetFortitudeSaveBonus();
        if(savingThrow > difficultyClass)
        {
            combatant->speedPenalty += 0;
        }
        else
        {
            combatant->speedPenalty += 2;
        }
    }
}

void Condition::InitializeEvasion()
{
    duration = 1;
}

void Condition::ApplyEvasion() 
{   
}*/

#include "Condition.h"
#include "FlumenBattle/World/Character/Character.h"
//#include "FlumenBattle/Utility/Utility.h"

using namespace world::character;

namespace world::character
{
    class ModifierAccessor
    {
    public:
        static void AddModifier(Character &character, Modifier modifier)
        {
            character.AddModifier(modifier);
        }
    };

    class Paralyzed : public ConditionType
    {
        using ConditionType::ConditionType; 

        void HandleApplyEffect(Character &character) const override
        {
            ModifierAccessor::AddModifier(character, {Modifiers::ALL_ROLLS_PENALTY, 1});
        }
    };

    class Sickened : public ConditionType
    {
        using ConditionType::ConditionType; 

        void HandleApplyEffect(Character &character) const override
        {
            ModifierAccessor::AddModifier(character, {Modifiers::ALL_ROLLS_PENALTY, 1});
        }
    };

    class Frightened : public ConditionType
    {
        using ConditionType::ConditionType; 

        void HandleApplyEffect(Character &character) const override
        {
            ModifierAccessor::AddModifier(character, {Modifiers::ALL_ROLLS_PENALTY, 1});
        }
    };

    class Fatigue : public ConditionType
    {
        using ConditionType::ConditionType; 

        void HandleApplyEffect(Character &character) const override
        {
            ModifierAccessor::AddModifier(character, {Modifiers::ALL_ROLLS_PENALTY, 1});
        }
    };

    class Nourished : public ConditionType
    {
        using ConditionType::ConditionType; 

        void HandleApplyEffect(Character &character) const override
        {
            ModifierAccessor::AddModifier(character, {Modifiers::DAMAGE_BONUS, 1});
            ModifierAccessor::AddModifier(character, {Modifiers::FORTITUDE_BONUS, 1});
        }
    };

    class Blessed : public ConditionType
    {
        using ConditionType::ConditionType; 

        void HandleApplyEffect(Character &character) const override
        {
            ModifierAccessor::AddModifier(character, {Modifiers::ATTACK_RATING_BONUS, 1});
        }
    };
}

void Condition::ApplyEffect(Character &character) const
{
    Type->HandleApplyEffect(character);
}

Condition ConditionFactory::Create(ConditionData data)
{
    switch(data.Type)
    {
    case Conditions::PARALYZED:
        return 
        {
            [&] {static const auto conditionType = Paralyzed(data.Type, "Paralyzed", false); return &conditionType;} (),
            1
        };
    case Conditions::FRIGHTENED:
        return 
        {
            [&] {static const auto conditionType = Frightened(data.Type, "Frightened", false); return &conditionType;} (), 
            1
        };
    case Conditions::SICKENED:
        return 
        {
            [&] {static const auto conditionType = Sickened(data.Type, "Sickened", false); return &conditionType;} (), 
            1
        };
    case Conditions::FATIGUE:
        return 
        {
            [&] {static const auto conditionType = Fatigue(data.Type, "Fatigue", false); return &conditionType;} (), 
            1
        };
    case Conditions::NOURISHED:
        return 
        {
            [&] {static const auto conditionType = Nourished(data.Type, "Nourished", true); return &conditionType;} (),
            1,
            data.Duration
        };
    case Conditions::BLESSED:
        return 
        {
            [&] {static const auto conditionType = Blessed(data.Type, "Blessed", true); return &conditionType;} (),
            data.Strength,
            data.Duration
        };
    }
}

bool ConditionManager::HasCondition(Conditions condition)
{
    return conditionSet.conditions.Find(condition) != nullptr;
}

void ConditionManager::AddCondition(ConditionData data)
{
    if(HasCondition(data.Type) == true)
        return;

    const auto &condition = ConditionFactory::Get()->Create(data);

    auto conditionPointer = conditionSet.conditions.Add();

    *conditionPointer = condition;
}

void ConditionManager::RemoveCondition(Conditions type)
{
    auto condition = conditionSet.conditions.Find(type);
    if(condition == nullptr)
        return;

    conditionSet.conditions.RemoveAt(condition);
}

void ConditionManager::ApplyModifiers(Character &character) const
{
    for(const auto &condition : conditionSet.Get())
    {
        condition.ApplyEffect(character);
    }
}

void ConditionManager::Update()
{
    for(auto &condition : conditionSet.conditions)
    {
        if(condition.Type->IsTimeDependent == false)
            continue;

        condition.TimeElapsed++;
        if(condition.TimeElapsed > condition.Duration)
        {
            conditionSet.conditions.RemoveAt(&condition);
        }
    }
}