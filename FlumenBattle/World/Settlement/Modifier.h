#pragma once

#include "FlumenCore/Container/Array.hpp"

#include "FlumenBattle/World/Settlement/Types.h"

namespace world::settlement
{
    class Settlement;

    struct Modifier
    {
        Modifiers Type;

        int Amount;

        bool operator==(Modifiers type) {return Type == type;}
    };

    class ModifierSet
    {
    protected:
        container::Array <Modifier> modifiers;

        ModifierSet() {}
    };

    class ModifierManager : protected ModifierSet
    {
        friend class ModifierAllocator;

        friend class Settlement;

        void AddModifier(Modifier);

        void ClearModifiers();

        int GetAmount(Modifiers);
    };

    class ModifierAllocator
    {
        friend class SettlementAllocator;

        static void Allocate(container::ArrayAllocator <Modifier> &allocator, ModifierManager &manager) 
        {
            manager.modifiers.Initialize(allocator);
        }
    };
}