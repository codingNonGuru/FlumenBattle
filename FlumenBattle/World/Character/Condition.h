#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenBattle/Types.hpp"

namespace battle
{
    class Combatant;
    class SpellCaster;
}

namespace world::character
{
    class Character;
}

#define ADD_FRIEND_CLASSES friend class world::character::Character;\
    friend class battle::Combatant;\
    friend class battle::SpellCaster;\
    friend class container::Pool <Condition>;\

namespace world::character
{
    class Condition
    {
        ADD_FRIEND_CLASSES

        ConditionTypes type;

        Integer duration;

        Integer difficultyClass;

        battle::Combatant *combatant;

        Condition();

        Condition(ConditionTypes, battle::Combatant *, Integer = 0);

        void InitializeCold();

        void InitializeEvasion();

        void Apply();

        void ApplyCold();

        void ApplyEvasion();

        ConditionTypes GetType() const {return type;}

        bool IsActive() const {return duration > 0;}
    };
}