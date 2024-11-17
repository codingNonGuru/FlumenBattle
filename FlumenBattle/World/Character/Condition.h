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
#include "FlumenCore/Conventions.hpp"
#include "FlumenCore/Singleton.h"

#include "FlumenBattle/World/Character/Types.h"

namespace battle
{
    class Combatant;
}

namespace world::character
{
    class Character;
    class ModifierManager;

    struct ConditionData
    {
        Conditions Type;

        int Duration;

        int Strength;

        int TimeInitiated {0};
    };

    struct ConditionType
    {
        Conditions Type;

        bool IsTimeDependent;

        ShortWord Name;

        bool IsBattleSpecific;

        explicit ConditionType(Conditions type, ShortWord name) : Type(type), Name(name) {}

        explicit ConditionType(Conditions type, ShortWord name, bool isTimeDependent) : Type(type), Name(name), IsTimeDependent(isTimeDependent) {}

        virtual void HandleApplyEffect(Character &) const {}

        virtual void HandleApplyEffect(battle::Combatant &) const {}
    };

    struct Condition
    {
        const ConditionType *Type {nullptr};

        int Strength;

        int Duration {0};

        int TimeElapsed {0};

        void ApplyEffect(Character &) const;

        void ApplyEffect(battle::Combatant &) const;

        bool operator ==(Conditions condition) {return Type->Type == condition;}

        Condition() {}

        Condition(const ConditionType *type, int strength) : Type(type), Strength(strength) {}

        Condition(const ConditionType *type, int strength, int duration) : Type(type), Strength(strength), Duration(duration) {}
    };

    struct ConditionSet
    {
        friend class ConditionAllocator;

        friend class ConditionManager;

    private:
        container::Pool <Condition> conditions;

        ConditionSet() {}

        //ConditionSet& operator=(const ConditionSet &) = delete;

        const container::Pool <Condition> &Get() const {return conditions;}
    };

    class ConditionManager
    {
        ConditionSet conditionSet;

        friend class Character;

        friend class battle::Combatant;

        friend class ConditionAllocator;

        bool HasCondition(Conditions);

        void AddCondition(ConditionData);

        void RemoveCondition(Conditions);

        void ApplyModifiers(Character &) const;

        void ApplyModifiers(battle::Combatant &) const;

        void Update();

        void UpdateBattle(int);

        const container::Pool <Condition> &GetConditions() const {return conditionSet.Get();}
    };

    class ConditionAllocator
    {
        friend class CharacterAllocator;

        friend class battle::Combatant;

        static void Initialize(ConditionManager &manager, int capacity)
        {
            manager.conditionSet.conditions.Initialize(capacity);
        }

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

        Condition Create(ConditionData);
    };
}