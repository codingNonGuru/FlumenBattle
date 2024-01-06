#include "FlumenCore/Container/Array.hpp"

#include "FlumenBattle/World/GroupAllocator.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/WorldGenerator.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/World/Group/Encounter.h"

#define CHARACTERS_PER_GROUP 16

#define ITEMS_PER_GROUP 64

namespace world::group
{
    void GroupAllocator::PreallocateMaximumMemory()
    {
        auto groupCount = WorldGenerator::Get()->GetMaximumGroupCount(MAXIMUM_WORLD_SIZE);
        groupMemory = container::Pool <Group>::PreallocateMemory(groupCount);

        characterMemory = container::PoolAllocator <character::Character>::PreallocateMemory(groupCount, CHARACTERS_PER_GROUP);

        battleMemory = container::Pool <Encounter>::PreallocateMemory(groupCount);

        itemMemory = container::PoolAllocator <character::Item>::PreallocateMemory(groupCount, ITEMS_PER_GROUP);
    }

    void GroupAllocator::AllocateWorldMemory(int worldSize)
    {
        auto groupCount = WorldGenerator::Get()->GetMaximumGroupCount(worldSize);
        groups.Initialize(groupCount, groupMemory);

        characterAllocator = container::PoolAllocator <character::Character> (groupCount, CHARACTERS_PER_GROUP, characterMemory);

        battles = container::Pool <Encounter> (groupCount, battleMemory);

        itemAllocator = container::PoolAllocator <character::Item> (groupCount, ITEMS_PER_GROUP);
    }

    Group * GroupAllocator::Allocate()
    {
        auto group = groups.Add();

        group->GetCharacters().Initialize(characterAllocator);

        character::ItemAllocator::Allocate(itemAllocator, group->items);

        return group;
    }

    void GroupAllocator::PerformCleanup()
    {
        static auto finishedGroups = Array <Group *> (groups.GetCapacity());

        for(auto &group : groups)
        {
            if(group.IsAlive())
                continue;
            
            *finishedGroups.Allocate() = &group;
        }

        for(auto group : finishedGroups)
        {
            groups.Remove(group);

            group->GetCharacters().Terminate(characterAllocator);

            character::ItemAllocator::Free(itemAllocator, group->items);
        }

        finishedGroups.Reset();
    }
}