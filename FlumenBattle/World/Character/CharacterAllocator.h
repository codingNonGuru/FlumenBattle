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
        class Group;
    }

    namespace character
    {
        struct CharacterAction;
        class Character;

        class CharacterAllocator : public core::Singleton<CharacterAllocator>
        {
            container::PoolAllocator <Weapon> weaponAllocator;

            container::PoolAllocator <Spell> spellAllocator;

            container::PoolAllocator <SpellSlot> spellSlotAllocator;

            container::ArrayAllocator <CharacterAction> actionAllocator;

        public:
            CharacterAllocator();

            Character * Allocate(group::Group &);
        };
    }
}