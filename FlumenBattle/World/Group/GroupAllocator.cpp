#include <mutex>

#include "FlumenCore/Container/Array.hpp"

#include "FlumenBattle/World/Group/GroupAllocator.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/WorldGenerator.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/World/Character/CharacterAllocator.h"
#include "FlumenBattle/World/Group/Encounter.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Group/GroupBatchMap.h"
#include "FlumenBattle/World/Group/GroupBatch.h"

#define CHARACTERS_PER_GROUP 16

#define ITEMS_PER_GROUP 64

static auto lastUniqueId = 0;

namespace world::group
{
    void GroupAllocator::PreallocateMaximumMemory()
    {
        std::cout<<"Memory size of a Character is "<<sizeof(character::Character)<<"\n";
        std::cout<<"Memory size of a Group is "<<sizeof(Group)<<"\n";
        std::cout<<"Memory size of a Group + dependencies is "<<sizeof(Group) + sizeof(character::Character) * CHARACTERS_PER_GROUP + sizeof(character::Item) * ITEMS_PER_GROUP<<"\n";

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

        itemAllocator = container::PoolAllocator <character::Item> (groupCount, ITEMS_PER_GROUP, itemMemory);
    }

    Group * GroupAllocator::Allocate()
    {
        static std::mutex mutex;

        mutex.lock();

        auto group = groups.Add();

        group->uniqueId = lastUniqueId;
        lastUniqueId++;

        group->characters.Initialize(characterAllocator);

        character::ItemAllocator::Allocate(itemAllocator, group->items);

        mutex.unlock();

        return group;
    }

    void GroupAllocator::Free(Group *group, bool willRemoveFromHome)
    {
        static std::mutex mutex;

        mutex.lock();

        auto batch = GroupBatchMap::Get()->GetBatch(group->GetTile());
        if(batch != nullptr)
        {
            batch->Remove(group);
        }

        if(group->home != nullptr && willRemoveFromHome == true)
        {
            group->home->RemoveGroup(*group);
        }

        for(auto &character : group->characters)
        {
            character::CharacterAllocator::Get()->Free(&character);
        }

        group->characters.Terminate(characterAllocator);

        character::ItemAllocator::Free(itemAllocator, group->items);

        groups.RemoveAt(group);

        mutex.unlock();
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

        for(auto &group : finishedGroups)
        {
            Free(group, true);
        }

        finishedGroups.Reset();
    }
}