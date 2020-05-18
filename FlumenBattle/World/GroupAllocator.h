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
    class GroupAllocator : public Singleton<GroupAllocator>
    {
        friend class WorldScene;

        friend class group::GroupFactory;

        container::Pool <group::Group> groups;

        container::PoolAllocator <Character> characterAllocator;

        void PerformCleanup();

        group::Group *Allocate();

        container::PoolAllocator <Character> &GetCharacterAllocator() {return characterAllocator;}

    public:
        GroupAllocator();
    };
}