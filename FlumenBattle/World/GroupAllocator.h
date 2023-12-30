#pragma once

#include "FlumenCore/Container/PoolAllocator.h"
#include "FlumenCore/Container/Pool.hpp"

#include "FlumenCore/Singleton.h"

class Character;
namespace world::group
{
    class Group;
    class GroupFactory;
    class Encounter;
}

namespace world
{   
    class GroupAllocator : public core::Singleton<GroupAllocator>
    {
        friend class WorldScene;

        friend class group::GroupFactory;

        container::Pool <group::Group>::Memory groupMemory;

        container::PoolAllocator <Character>::Memory characterMemory;

        container::Pool <group::Encounter>::Memory battleMemory;

        container::Pool <group::Group> groups;

        container::PoolAllocator <Character> characterAllocator;

        container::Pool <group::Encounter> battles;

        void PerformCleanup();

        group::Group *Allocate();

        container::PoolAllocator <Character> &GetCharacterAllocator() {return characterAllocator;}

    public:
        void PreallocateMaximumMemory();

        void AllocateWorldMemory(int);

        container::Pool <group::Group> *GetGroups() {return &groups;}

        container::Pool <group::Encounter> *GetBattles() {return &battles;}
    };
}