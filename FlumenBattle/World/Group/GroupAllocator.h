#pragma once

#include "FlumenCore/Container/PoolAllocator.h"
#include "FlumenCore/Container/Pool.hpp"
#include "FlumenCore/Container/SmartBlock.hpp"

#include "FlumenCore/Singleton.h"

namespace world::character
{
    class Character;
    class Item;
}

namespace world::group
{   
    class GroupCore;
    class GroupExtraData;
    class GroupFactory;
    class Encounter;
    struct CharacterEssence;
    struct TravelActionData;
    typedef container::SmartBlock <CharacterEssence, 16> CharacterEssenceBatch;

    class GroupAllocator : public core::Singleton <GroupAllocator>
    {
        friend class GroupCore;

        container::Pool <GroupCore>::Memory groupMemory;

        container::Pool <GroupExtraData>::Memory extraGroupDataMemory;

        container::PoolAllocator <character::Character>::Memory characterMemory;

        container::Pool <CharacterEssenceBatch>::Memory characterEssenceMemory;

        container::Pool <Encounter>::Memory battleMemory;

        container::PoolAllocator <character::Item>::Memory itemMemory;

        container::Pool <TravelActionData>::Memory travelDataMemory;


        container::Pool <GroupCore> groups;

        container::Pool <GroupExtraData> extraGroupDatas;

        container::PoolAllocator <character::Character> characterAllocator;

        container::Pool <CharacterEssenceBatch> characterEssenceAllocator;

        container::Pool <Encounter> battles;

        container::PoolAllocator <character::Item> itemAllocator;

        container::Pool <TravelActionData> travelDataAllocator;

        container::PoolAllocator <character::Character> &GetCharacterAllocator() {return characterAllocator;}

        GroupExtraData *AllocateExtraData();

        void FreeExtraData(GroupExtraData *);

    public:
        GroupCore *Allocate(bool);

        void GenerateExtraGroupData(GroupCore *);

        void PreallocateMaximumMemory();

        void AllocateWorldMemory(int);

        container::Pool <GroupCore> *GetGroups() {return &groups;}

        container::Pool <Encounter> *GetBattles() {return &battles;}

        void Free(GroupCore *, bool);

        void PerformCleanup();
    };
}