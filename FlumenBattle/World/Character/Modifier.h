#pragma once

#include "FlumenCore/Container/Array.hpp"

#include "FlumenBattle/World/Character/Types.h"

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

        void Initialize();

        void AddModifier(Modifier);

        void ClearModifiers();

        int GetAmount(Modifiers) const;
    };

    class ModifierAllocator
    {
        friend class CharacterAllocator;

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