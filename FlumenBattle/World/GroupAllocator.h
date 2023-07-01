#pragma once

#include "FlumenCore/Container/PoolAllocator.h"
#include "FlumenCore/Container/Pool.hpp"

#include "FlumenEngine/Core/Singleton.h"

class Character;
namespace world::group
{
    class Group;
    class GroupFactory;
}

namespace world
{
    class Battle;
    
    class GroupAllocator : public Singleton<GroupAllocator>
    {
        friend class WorldScene;

        friend class group::GroupFactory;

        container::Pool <group::Group>::Memory groupMemory;

        container::PoolAllocator <Character>::Memory characterMemory;

        container::Pool <Battle>::Memory battleMemory;

        container::Pool <group::Group> groups;

        container::PoolAllocator <Character> characterAllocator;

        container::Pool <Battle> battles;

        void PerformCleanup();

        group::Group *Allocate();

        container::PoolAllocator <Character> &GetCharacterAllocator() {return characterAllocator;}

    public:
        void PreallocateMaximumMemory();

        void AllocateWorldMemory(int);

        container::Pool <group::Group> *GetGroups() {return &groups;}

        container::Pool <Battle> *GetBattles() {return &battles;}
    };
}