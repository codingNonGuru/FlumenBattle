#pragma once

#include "FlumenCore/Container/Array.hpp"

#include "FlumenBattle/World/Character/Types.h"

namespace battle
{
    class Combatant;
}

namespace world::character
{
    class Character;

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

        friend class Character;

        friend class battle::Combatant;

        void Initialize();

        void AddModifier(Modifier);

        void ClearModifiers();

        int GetAmount(Modifiers) const;
    };

    class ModifierAllocator
    {
        friend class CharacterAllocator;

        friend class battle::Combatant;

        static void Initialize(ModifierManager &manager, int capacity) 
        {
            manager.modifiers.Initialize(capacity);
        }

        static void Allocate(container::ArrayAllocator <Modifier> &allocator, ModifierManager &manager) 
        {
            manager.modifiers.Initialize(allocator);
        }

        static void Free(container::ArrayAllocator <Modifier> &allocator, ModifierManager &manager) 
        {
            manager.modifiers.Terminate(allocator);
        }
    };
}