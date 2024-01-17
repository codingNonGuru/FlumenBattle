/*#pragma once

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
}*/

#pragma once

#include "FlumenCore/Container/Pool.hpp"
#include "FlumenCore/Container/PoolAllocator.h"

#include "FlumenCore/Singleton.h"

#include "FlumenBattle/World/Character/Types.h"

namespace world::character
{
    class Character;
    class ModifierManager;

    struct ConditionData
    {
        Conditions Type;

        int Strength;

        int Duration;
    };

    struct ConditionType
    {
        Conditions Type;

        explicit ConditionType(Conditions type) : Type(type) {}

        virtual void HandleApplyEffect(Character &) const = 0;
    };

    struct Condition
    {
        const ConditionType *Type;

        int Strength;

        int Duration;

        int HoursElapsed;

        void ApplyEffect(Character &character) const;

        bool operator ==(Conditions condition) {return Type->Type == condition;}
    };

    struct ConditionSet
    {
        friend class ConditionAllocator;

        friend class ConditionManager;

    private:
        container::Pool <Condition> conditions;

        ConditionSet() {}

        ConditionSet& operator=(const ConditionSet &) = delete;

        const container::Pool <Condition> &Get() const {return conditions;}
    };

    class ConditionManager
    {
        ConditionSet conditionSet;

        friend class Character;

        friend class ConditionAllocator;

        bool HasCondition(Conditions);

        void AddCondition(ConditionData);

        void RemoveCondition(Conditions);

        void ApplyModifiers(Character &) const;

        void Update();
    };

    class ConditionAllocator
    {
        friend class CharacterAllocator;

        static void Allocate(container::PoolAllocator <Condition> &allocator, ConditionManager &manager) 
        {
            manager.conditionSet.conditions.Initialize(allocator);
        }

        static void Free(container::PoolAllocator <Condition> &allocator, ConditionManager &manager) 
        {
            manager.conditionSet.conditions.Terminate(allocator);
        }
    };

    class ConditionFactory : public core::Singleton <ConditionFactory>
    {
        friend class ConditionManager;

        Condition Create(Conditions);
    };
}