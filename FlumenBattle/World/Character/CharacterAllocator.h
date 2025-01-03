#pragma once

#include "FlumenCore/Container/PoolAllocator.h"
#include "FlumenCore/Container/ArrayAllocator.h"

#include "FlumenCore/Singleton.h"

struct Weapon;
struct Spell;
struct SpellSlot;

namespace world
{
    namespace group
    {
        class GroupCore;
    }

    namespace character
    {
        struct CharacterAction;
        class Character;
        class Condition;
        class Modifier;

        class CharacterAllocator : public core::Singleton <CharacterAllocator>
        {
            container::PoolAllocator <Condition>::Memory conditionMemory;

            container::ArrayAllocator <Modifier>::Memory modifierMemory;


            container::PoolAllocator <Condition> conditionAllocator;

            container::ArrayAllocator <Modifier> modifierAllocator;

        public:
            void AllocateWorldMemory(int);

            void PreallocateMaximumMemory();

            Character *Allocate(group::GroupCore &);

            void Free(Character *);
        };
    }
}