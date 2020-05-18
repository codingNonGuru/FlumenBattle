#include "FlumenBattle/World/CharacterAllocator.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/Character.h"
#include "FlumenBattle/Spell.h"
#include "FlumenBattle/Weapon.h"

namespace world
{
    CharacterAllocator::CharacterAllocator()
    {
        weaponAllocator = container::PoolAllocator <Weapon> (16 * 64, 4);

        spellAllocator = container::PoolAllocator <Spell> (16 * 64, 8);

        spellSlotAllocator = container::PoolAllocator <SpellSlot> (16 * 64, 8);
    }

    Character * CharacterAllocator::Allocate(group::Group &group)
    {
        auto &characters = group.GetCharacters();
        auto character = characters.Add(); 
        character->weapons.Initialize(weaponAllocator);
        character->spells.Initialize(spellAllocator);
        character->spellSlots.Initialize(spellSlotAllocator);
        return character;
    }
}