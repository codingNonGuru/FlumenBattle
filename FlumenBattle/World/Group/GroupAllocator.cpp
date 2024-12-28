#include <mutex>

#include "FlumenCore/Container/Array.hpp"

#include "FlumenBattle/World/Group/GroupAllocator.h"
#include "FlumenBattle/World/Group/GroupCore.h"
#include "FlumenBattle/World/Group/GroupExtraData.h"
#include "FlumenBattle/World/Group/CharacterEssence.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/WorldGenerator.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/World/Character/CharacterAllocator.h"
#include "FlumenBattle/World/Group/Encounter.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Group/GroupBatchMap.h"
#include "FlumenBattle/World/Group/GroupBatch.h"
#include "FlumenBattle/Config.h"

#define ITEMS_PER_GROUP 64

static auto lastUniqueId = 0;

namespace world::group
{
    void GroupAllocator::PreallocateMaximumMemory()
    {
        static const auto MAXIMUM_WORLD_SIZE = engine::ConfigManager::Get()->GetValue(game::ConfigValues::MAXIMUM_WORLD_SIZE).Integer;
        
        static const auto MAXIMUM_CHARACTERS_PER_GROUP = engine::ConfigManager::Get()->GetValue(game::ConfigValues::MAXIMUM_CHARACTERS_PER_GROUP).Integer;

        std::cout<<"Memory size of a Character is "<<sizeof(character::Character)<<"\n";
        std::cout<<"Memory size of a Group is "<<sizeof(GroupCore)<<"\n";
        std::cout<<"Memory size of a Group + dependencies is "<<sizeof(GroupCore) + sizeof(character::Character) * MAXIMUM_CHARACTERS_PER_GROUP + sizeof(character::Item) * ITEMS_PER_GROUP<<"\n";

        auto groupCount = WorldGenerator::Get()->GetMaximumGroupCount(MAXIMUM_WORLD_SIZE);
        groupMemory = container::Pool <GroupCore>::PreallocateMemory(groupCount);

        auto extraDataCount = WorldGenerator::Get()->GetMaximumGroupCount(MAXIMUM_WORLD_SIZE / 4);
        extraGroupDataMemory = container::Pool <GroupExtraData>::PreallocateMemory(extraDataCount);

        characterMemory = container::PoolAllocator <character::Character>::PreallocateMemory(extraDataCount, MAXIMUM_CHARACTERS_PER_GROUP);

        characterEssenceMemory = container::Pool <CharacterEssenceBatch>::PreallocateMemory(groupCount);

        battleMemory = container::Pool <Encounter>::PreallocateMemory(groupCount);

        itemMemory = container::PoolAllocator <character::Item>::PreallocateMemory(extraDataCount, ITEMS_PER_GROUP);

        travelDataMemory = container::Pool <TravelActionData>::PreallocateMemory(groupCount);
    }

    void GroupAllocator::AllocateWorldMemory(int worldSize)
    {
        static const auto MAXIMUM_CHARACTERS_PER_GROUP = engine::ConfigManager::Get()->GetValue(game::ConfigValues::MAXIMUM_CHARACTERS_PER_GROUP).Integer;

        auto groupCount = WorldGenerator::Get()->GetMaximumGroupCount(worldSize);
        groups.Initialize(groupCount, groupMemory);

        auto extraDataCount = WorldGenerator::Get()->GetMaximumGroupCount(worldSize / 4);
        extraGroupDatas.Initialize(extraDataCount, extraGroupDataMemory);

        characterAllocator = container::PoolAllocator <character::Character> (extraDataCount, MAXIMUM_CHARACTERS_PER_GROUP, characterMemory);

        characterEssenceAllocator = container::Pool <CharacterEssenceBatch> (groupCount, characterEssenceMemory);

        battles = container::Pool <Encounter> (groupCount, battleMemory);

        itemAllocator = container::PoolAllocator <character::Item> (extraDataCount, ITEMS_PER_GROUP, itemMemory);

        travelDataAllocator = container::Pool <TravelActionData> (groupCount, travelDataMemory);
    }

    GroupCore * GroupAllocator::Allocate(bool hasExtraData)
    {
        auto group = groups.Add();

        group->uniqueId = lastUniqueId;
        lastUniqueId++;

        group->travelActionData = travelDataAllocator.Add();

        if(hasExtraData == true)
        {
            group->extraData = AllocateExtraData();
        }
        else
        {
            group->extraData = nullptr;

            auto characterEssences = characterEssenceAllocator.Add();
            group->characterHandler.characters = characterEssences;
        }

        return group;
    }

    GroupExtraData *GroupAllocator::AllocateExtraData()
    {
        auto extraData = extraGroupDatas.Add();

        extraData->characters.Initialize(characterAllocator);

        character::ItemAllocator::Allocate(itemAllocator, extraData->items);

        return extraData;
    }

    void GroupAllocator::GenerateExtraGroupData(GroupCore *group)
    {
        group->extraData = AllocateExtraData();
    }

    void GroupAllocator::Free(GroupCore *group, bool willRemoveFromHome)
    {
        auto batch = GroupBatchMap::Get()->GetBatch(group->GetTile());
        if(batch != nullptr)
        {
            batch->Remove(group);
        }

        if(group->GetHome() != nullptr && willRemoveFromHome == true)
        {
            group->GetHome()->RemoveGroup(*group);
        }

        travelDataAllocator.RemoveAt(group->travelActionData);

        characterEssenceAllocator.RemoveAt(group->characterHandler.characters);

        if(group->extraData != nullptr)
        {
            FreeExtraData(group->extraData);
        }

        groups.RemoveAt(group);
    }

    void GroupAllocator::FreeExtraData(GroupExtraData *extraData)
    {
        for(auto &character : extraData->characters)
        {
            character::CharacterAllocator::Get()->Free(&character);
        }

        extraData->characters.Terminate(characterAllocator);

        character::ItemAllocator::Free(itemAllocator, extraData->items);
    }

    void GroupAllocator::PerformCleanup()
    {
        static auto finishedGroups = Array <GroupCore *> (groups.GetCapacity());

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