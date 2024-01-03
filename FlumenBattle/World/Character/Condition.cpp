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
            ModifierAccessor::AddModifier(character, {Modifiers::DAMAGE_INCREASE, 1});
        }
    };

    class Sickened : public ConditionType
    {
        using ConditionType::ConditionType; 

        void HandleApplyEffect(Character &character) const override
        {
            ModifierAccessor::AddModifier(character, {Modifiers::DAMAGE_INCREASE, 1});
        }
    };

    class Frightened : public ConditionType
    {
        using ConditionType::ConditionType; 

        void HandleApplyEffect(Character &character) const override
        {
            ModifierAccessor::AddModifier(character, {Modifiers::DAMAGE_INCREASE, 1});
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
}

void Condition::ApplyEffect(Character &character) const
{
    Type->HandleApplyEffect(character);
}

Condition ConditionFactory::Create(Conditions type)
{
    switch(type)
    {
    case Conditions::PARALYZED:
        return 
        {
            [&] {static const auto conditionType = Paralyzed(type); return &conditionType;} (), 
            false,
            true
        };
    case Conditions::FRIGHTENED:
        return 
        {
            [&] {static const auto conditionType = Frightened(type); return &conditionType;} (), 
            false,
            false
        };
    case Conditions::SICKENED:
        return 
        {
            [&] {static const auto conditionType = Sickened(type); return &conditionType;} (), 
            false,
            false
        };
    case Conditions::FATIGUE:
        return 
        {
            [&] {static const auto conditionType = Fatigue(type); return &conditionType;} (), 
            false,
            false
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

    const auto &condition = ConditionFactory::Get()->Create(data.Type);

    auto conditionPointer = conditionSet.conditions.Add();

    *conditionPointer = condition;
}

void ConditionManager::RemoveCondition(Conditions type)
{
    auto condition = conditionSet.conditions.Find(type);
    if(condition == nullptr)
        return;

    conditionSet.conditions.Remove(condition);
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

}