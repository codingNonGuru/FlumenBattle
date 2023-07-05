#include "FlumenCore/Container/Array.hpp"

#include "FlumenBattle/World/GroupAllocator.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/WorldGenerator.h"
#include "FlumenBattle/Character.h"
#include "FlumenBattle/World/Group/Encounter.h"

#define CHARACTERS_PER_GROUP 16

namespace world
{
    void GroupAllocator::PreallocateMaximumMemory()
    {
        auto groupCount = WorldGenerator::Get()->GetMaximumGroupCount(MAXIMUM_WORLD_SIZE);
        groupMemory = container::Pool <group::Group>::PreallocateMemory(groupCount);

        characterMemory = container::PoolAllocator <Character>::PreallocateMemory(groupCount, CHARACTERS_PER_GROUP);

        battleMemory = container::Pool <group::Encounter>::PreallocateMemory(groupCount);
    }

    void GroupAllocator::AllocateWorldMemory(int worldSize)
    {
        auto groupCount = WorldGenerator::Get()->GetMaximumGroupCount(worldSize);
        groups.Initialize(groupCount, groupMemory);

        characterAllocator = container::PoolAllocator <Character> (groupCount, CHARACTERS_PER_GROUP, characterMemory);

        battles = container::Pool <group::Encounter> (groupCount, battleMemory);
    }

    group::Group * GroupAllocator::Allocate()
    {
        auto group = groups.Add();

        group->GetCharacters().Initialize(characterAllocator);

        return group;
    }

    void GroupAllocator::PerformCleanup()
    {
        static auto finishedGroups = Array <group::Group *> (groups.GetCapacity());

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
        }

        finishedGroups.Reset();
    }
}