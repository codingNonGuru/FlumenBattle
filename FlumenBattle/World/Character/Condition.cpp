#include "Condition.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/Battle/Combatant.h"
#include "FlumenBattle/Battle/BattleScene.h"

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

        static void AddModifier(battle::Combatant &combatant, Modifier modifier)
        {
            combatant.AddModifier(modifier);
        }
    };

    class Paralyzed : public ConditionType
    {
        using ConditionType::ConditionType; 

        void HandleApplyEffect(Character &character, const Condition *condition) const override
        {
            ModifierAccessor::AddModifier(character, {Modifiers::ALL_ROLLS_PENALTY, 1});
        }
    };

    class Sickened : public ConditionType
    {
        using ConditionType::ConditionType; 

        void HandleApplyEffect(Character &character, const Condition *condition) const override
        {
            ModifierAccessor::AddModifier(character, {Modifiers::ALL_ROLLS_PENALTY, 1});
        }
    };

    class Frightened : public ConditionType
    {
        using ConditionType::ConditionType; 

        void HandleApplyEffect(Character &character, const Condition *condition) const override
        {
            ModifierAccessor::AddModifier(character, {Modifiers::ALL_ROLLS_PENALTY, 1});
        }
    };

    class Fatigue : public ConditionType
    {
        using ConditionType::ConditionType; 

        void HandleApplyEffect(Character &character, const Condition *condition) const override
        {
            ModifierAccessor::AddModifier(character, {Modifiers::ALL_ROLLS_PENALTY, 1});
        }
    };

    class Nourished : public ConditionType
    {
        using ConditionType::ConditionType; 

        void HandleApplyEffect(Character &character, const Condition *condition) const override
        {
            ModifierAccessor::AddModifier(character, {Modifiers::DAMAGE_BONUS, 1});
            ModifierAccessor::AddModifier(character, {Modifiers::FORTITUDE_BONUS, 1});
        }
    };

    class Blessed : public ConditionType
    {
        using ConditionType::ConditionType; 

        void HandleApplyEffect(battle::Combatant &combatant, const Condition *condition) const override
        {
            ModifierAccessor::AddModifier(combatant, {Modifiers::ATTACK_RATING_BONUS, 1});
        }
    };

    class Hobbled : public ConditionType
    {
        using ConditionType::ConditionType; 

        void HandleApplyEffect(battle::Combatant &combatant, const Condition *condition) const override
        {
            ModifierAccessor::AddModifier(combatant, {Modifiers::MOVE_SPEED, -2});
        }
    };

    class Surprised : public ConditionType
    {
        using ConditionType::ConditionType; 

        void HandleApplyEffect(Character &character, const Condition *condition) const override
        {
            ModifierAccessor::AddModifier(character, {Modifiers::INITIATIVE_PENALTY, condition->Strength});
        }
    };

    class Alert : public ConditionType
    {
        using ConditionType::ConditionType; 

        void HandleApplyEffect(Character &character, const Condition *condition) const override
        {
            ModifierAccessor::AddModifier(character, {Modifiers::INITIATIVE_BONUS, condition->Strength});
        }
    };

    class WallProtection : public ConditionType
    {
        using ConditionType::ConditionType; 

        void HandleApplyEffect(battle::Combatant &combatant, const Condition *condition) const override
        {
            if(condition->Strength == 1)
            {
                ModifierAccessor::AddModifier(combatant, {Modifiers::BONUS_ARMOR_CLASS, 2});
            }
            else if(condition->Strength == 2)
            {
                ModifierAccessor::AddModifier(combatant, {Modifiers::BONUS_ARMOR_CLASS, 3});

                ModifierAccessor::AddModifier(combatant, {Modifiers::FORTITUDE_BONUS, 1});

                ModifierAccessor::AddModifier(combatant, {Modifiers::REFLEX_BONUS, 1});
            }
            else if(condition->Strength == 3)
            {
                ModifierAccessor::AddModifier(combatant, {Modifiers::BONUS_ARMOR_CLASS, 4});

                ModifierAccessor::AddModifier(combatant, {Modifiers::FORTITUDE_BONUS, 2});

                ModifierAccessor::AddModifier(combatant, {Modifiers::REFLEX_BONUS, 2});
            }
        }
    };
}

void Condition::ApplyEffect(Character &character) const
{
    Type->HandleApplyEffect(character, this);
}

void Condition::ApplyEffect(battle::Combatant &combatant) const
{
    Type->HandleApplyEffect(combatant, this);
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
    case Conditions::HOBBLED:
        return 
        {
            [&] {static const auto conditionType = Hobbled(data.Type, "Hobbled", true); return &conditionType;} (),
            data.Strength,
            data.Duration
        };
    case Conditions::WALL_PROTECTION:
        return 
        {
            [&] {static const auto conditionType = WallProtection(data.Type, "Wall protection", true); return &conditionType;} (),
            data.Strength,
            data.Duration
        };
    case Conditions::SURPRISED:
        return 
        {
            [&] {static const auto conditionType = Surprised(data.Type, "Surprised", false); return &conditionType;} (),
            data.Strength,
            data.Duration
        };
    case Conditions::ALERT:
        return 
        {
            [&] {static const auto conditionType = Alert(data.Type, "Alert", false); return &conditionType;} (),
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
    {
        return;
    }

    const auto &condition = ConditionFactory::Get()->Create(data);

    auto conditionPointer = conditionSet.conditions.Add();

    *conditionPointer = condition;

    if(condition.Type->IsTimeDependent == true)
    {
        conditionPointer->TimeElapsed = data.TimeInitiated;
    }
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

void ConditionManager::ApplyModifiers(battle::Combatant &combatant) const
{
    for(const auto &condition : conditionSet.Get())
    {
        condition.ApplyEffect(combatant);
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

void ConditionManager::UpdateBattle(int currentTurn)
{
    const auto turnsPerRound = battle::BattleScene::Get()->GetTurnsPerRound();

    for(auto &condition : conditionSet.conditions)
    {
        if(condition.Type->IsTimeDependent == false)
            continue;

        auto initialTurn = condition.TimeElapsed;

        auto turnsElapsed = currentTurn - initialTurn;

        if(turnsElapsed % turnsPerRound == 0 && turnsElapsed / turnsPerRound == condition.Duration)
        {
            conditionSet.conditions.RemoveAt(&condition);
        }
    }
}