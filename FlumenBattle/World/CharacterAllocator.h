#pragma once

#include "FlumenCore/Container/PoolAllocator.h"

#include "FlumenEngine/Core/Singleton.h"

struct Weapon;
struct Spell;
struct SpellSlot;
class Character;

namespace world
{
    namespace group
    {
        class Group;
    }

    class CharacterAllocator : public Singleton<CharacterAllocator>
    {
        container::PoolAllocator <Weapon> weaponAllocator;

        container::PoolAllocator <Spell> spellAllocator;

        container::PoolAllocator <SpellSlot> spellSlotAllocator;

    public:
        CharacterAllocator();

        Character * Allocate(group::Group &);
    };
}