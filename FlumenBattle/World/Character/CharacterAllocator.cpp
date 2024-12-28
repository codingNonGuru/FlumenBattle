#include "FlumenBattle/World/Character/CharacterAllocator.h"
#include "FlumenBattle/World/Group/GroupCore.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/World/Character/Spell.h"
#include "FlumenBattle/World/Character/Weapon.h"
#include "FlumenBattle/World/Character/CharacterAction.h"
#include "FlumenBattle/World/Character/CharacterClass.h"
#include "FlumenBattle/World/WorldGenerator.h"
#include "FlumenBattle/Config.h"

namespace world::character
{
    #define CONDITIONS_PER_CHARACTER 16

    #define MODIFIERS_PER_CHARACTER 16

    void CharacterAllocator::PreallocateMaximumMemory()
    {
        static const auto MAXIMUM_CHARACTERS_PER_GROUP = engine::ConfigManager::Get()->GetValue(game::ConfigValues::MAXIMUM_CHARACTERS_PER_GROUP).Integer;

        static const auto MAXIMUM_WORLD_SIZE = engine::ConfigManager::Get()->GetValue(game::ConfigValues::MAXIMUM_WORLD_SIZE).Integer;

        auto worldGenerator = WorldGenerator::Get();

        auto groupCount = worldGenerator->GetMaximumGroupCount(MAXIMUM_WORLD_SIZE);

        conditionMemory = container::PoolAllocator <Condition>::PreallocateMemory(groupCount * MAXIMUM_CHARACTERS_PER_GROUP, CONDITIONS_PER_CHARACTER);

        modifierMemory = container::ArrayAllocator <Modifier>::PreallocateMemory(groupCount * MAXIMUM_CHARACTERS_PER_GROUP, MODIFIERS_PER_CHARACTER);
    }

    void CharacterAllocator::AllocateWorldMemory(int worldSize)
    {
        static const auto MAXIMUM_CHARACTERS_PER_GROUP = engine::ConfigManager::Get()->GetValue(game::ConfigValues::MAXIMUM_CHARACTERS_PER_GROUP).Integer;
        
        auto worldGenerator = WorldGenerator::Get();

        auto groupCount = worldGenerator->GetMaximumGroupCount(worldSize);

        conditionAllocator = container::PoolAllocator <Condition> (MAXIMUM_CHARACTERS_PER_GROUP * groupCount, CONDITIONS_PER_CHARACTER);

        modifierAllocator = container::ArrayAllocator <Modifier> (MAXIMUM_CHARACTERS_PER_GROUP * groupCount, MODIFIERS_PER_CHARACTER);
    }

    Character * CharacterAllocator::Allocate(group::GroupCore &group)
    {
        auto &characters = group.GetCharacters();
        auto character = characters.Add(); 

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