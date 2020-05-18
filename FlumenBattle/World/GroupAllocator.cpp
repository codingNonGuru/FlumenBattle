#include "FlumenCore/Container/Array.hpp"

#include "FlumenBattle/World/GroupAllocator.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/Character.h"

#define MAXIMUM_GROUP_COUNT 64

#define CHARACTERS_PER_GROUP 16

namespace world
{
    GroupAllocator::GroupAllocator()
    {
        groups.Initialize(MAXIMUM_GROUP_COUNT);

        characterAllocator = container::PoolAllocator <Character> (MAXIMUM_GROUP_COUNT, CHARACTERS_PER_GROUP);
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