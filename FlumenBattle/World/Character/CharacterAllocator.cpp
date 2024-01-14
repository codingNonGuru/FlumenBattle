#include "FlumenBattle/World/Character/CharacterAllocator.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/World/Character/Spell.h"
#include "FlumenBattle/World/Character/Weapon.h"
#include "FlumenBattle/World/Character/CharacterAction.h"
#include "FlumenBattle/World/Character/CharacterClass.h"
#include "FlumenBattle/World/WorldGenerator.h"

namespace world::character
{
    #define MAXIMUM_GROUPS_PER_WORLD 64

    #define MAXIMUM_CHARACTERS_PER_GROUP 16

    #define CONDITIONS_PER_CHARACTER 16

    #define MODIFIERS_PER_CHARACTER 16

    void CharacterAllocator::PreallocateMaximumMemory()
    {
        auto worldGenerator = WorldGenerator::Get();

        auto groupCount = worldGenerator->GetMaximumGroupCount(MAXIMUM_WORLD_SIZE);

        conditionMemory = container::PoolAllocator <Condition>::PreallocateMemory(groupCount * MAXIMUM_CHARACTERS_PER_GROUP, CONDITIONS_PER_CHARACTER);

        modifierMemory = container::ArrayAllocator <Modifier>::PreallocateMemory(groupCount * MAXIMUM_CHARACTERS_PER_GROUP, MODIFIERS_PER_CHARACTER);
    }

    void CharacterAllocator::AllocateWorldMemory(int worldSize)
    {
        auto worldGenerator = WorldGenerator::Get();

        auto groupCount = worldGenerator->GetMaximumGroupCount(worldSize);

        //spellSlotAllocator = container::PoolAllocator <SpellSlot> (MAXIMUM_CHARACTERS_PER_GROUP * groupCount, 8);

        conditionAllocator = container::PoolAllocator <Condition> (MAXIMUM_CHARACTERS_PER_GROUP * groupCount, CONDITIONS_PER_CHARACTER);

        modifierAllocator = container::ArrayAllocator <Modifier> (MAXIMUM_CHARACTERS_PER_GROUP * groupCount, MODIFIERS_PER_CHARACTER);
    }

    Character * CharacterAllocator::Allocate(group::Group &group)
    {
        auto &characters = group.GetCharacters();
        auto character = characters.Add(); 
        //character->spellSlots.Initialize(spellSlotAllocator);

        ConditionAllocator::Allocate(conditionAllocator, character->conditions);

        ModifierAllocator::Allocate(modifierAllocator, character->modifiers);
        
        return character;
    }

    void CharacterAllocator::Free(Character *character)
    {
        ConditionAllocator::Free(conditionAllocator, character->conditions);

        ModifierAllocator::Free(modifierAllocator, character->modifiers);
    }
}