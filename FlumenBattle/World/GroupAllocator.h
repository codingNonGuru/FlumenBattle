#pragma once

#include "FlumenCore/Container/PoolAllocator.h"
#include "FlumenCore/Container/Pool.hpp"

#include "FlumenCore/Singleton.h"

namespace world
{
    class WorldScene;
}

namespace world::character
{
    class Character;
    class Item;
}

namespace world::group
{   
    class Group;
    class GroupFactory;
    class Encounter;

    class GroupAllocator : public core::Singleton <GroupAllocator>
    {
        friend class world::WorldScene;

        friend class GroupFactory;

        container::Pool <Group>::Memory groupMemory;

        container::PoolAllocator <character::Character>::Memory characterMemory;

        container::Pool <Encounter>::Memory battleMemory;

        container::PoolAllocator <character::Item>::Memory itemMemory;


        container::Pool <Group> groups;

        container::PoolAllocator <character::Character> characterAllocator;

        container::Pool <Encounter> battles;

        container::PoolAllocator <character::Item> itemAllocator;

        void PerformCleanup();

        Group *Allocate();

        container::PoolAllocator <character::Character> &GetCharacterAllocator() {return characterAllocator;}

    public:
        void PreallocateMaximumMemory();

        void AllocateWorldMemory(int);

        container::Pool <Group> *GetGroups() {return &groups;}

        container::Pool <Encounter> *GetBattles() {return &battles;}
    };
}