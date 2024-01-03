#include "FlumenBattle/World/Character/CharacterAllocator.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/World/Character/Spell.h"
#include "FlumenBattle/World/Character/Weapon.h"
#include "FlumenBattle/World/Character/CharacterAction.h"
#include "FlumenBattle/World/Character/CharacterClass.h"

namespace world::character
{
    CharacterAllocator::CharacterAllocator()
    {
        weaponAllocator = container::PoolAllocator <Weapon> (16 * 64, 4);

        spellAllocator = container::PoolAllocator <Spell> (16 * 64, 8);

        spellSlotAllocator = container::PoolAllocator <SpellSlot> (16 * 64, 8);

        actionAllocator = container::ArrayAllocator <character::CharacterAction> (16 * 64, 8);
    }

    Character * CharacterAllocator::Allocate(group::Group &group)
    {
        auto &characters = group.GetCharacters();
        auto character = characters.Add(); 
        character->weapons.Initialize(weaponAllocator);
        character->spells.Initialize(spellAllocator);
        character->spellSlots.Initialize(spellSlotAllocator);
        //character->type->Actions.Initialize(actionAllocator);
        return character;
    }
}