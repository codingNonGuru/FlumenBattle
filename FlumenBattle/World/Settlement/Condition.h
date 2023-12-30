#pragma once

#include "FlumenCore/Container/Pool.hpp"
#include "FlumenCore/Container/PoolAllocator.h"

#include "FlumenCore/Singleton.h"

#include "FlumenBattle/World/Types.h"

namespace world::settlement
{
    class Settlement;

    struct ConditionData
    {
        Conditions Type;

        int Strength;

        int Duration;
    };

    struct ConditionType
    {
        Conditions Type;

        void (*OnApply) (Settlement &);
    };

    struct Condition
    {
        const ConditionType *Type;

        int Strength;

        int Duration;

        int HoursElapsed;

        void Apply(Settlement &settlement) {if(Type->OnApply != nullptr) Type->OnApply(settlement);}

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

        container::Pool <Condition> &Get() {return &conditions;}
    };

    class ConditionManager
    {
        ConditionSet conditionSet;

        friend class Settlement;

        friend class ConditionAllocator;

        void AddCondition(ConditionData);

        void ApplyModifiers(Settlement &);

        void Update();
    };

    class ConditionAllocator
    {
        friend class SettlementAllocator;

        static void Allocate(container::PoolAllocator <Condition> &allocator, ConditionManager &manager) 
        {
            manager.conditionSet.conditions.Initialize(allocator);
        }
    };

    class ConditionFactory : public core::Singleton <ConditionFactory>
    {
        static void OnApplyMalariaImmunity(Settlement &);

        static void OnApplySickened(Settlement &);

        const ConditionType &BuildType(Conditions);

        const ConditionType &BuildMalariaImmunity();

        const ConditionType &BuildSickened();

    public:
        Condition Create(ConditionData);
    };
}