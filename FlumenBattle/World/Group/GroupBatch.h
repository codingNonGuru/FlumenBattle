#pragma once

#include "FlumenCore/Container/Pool.hpp"

namespace world
{
    class WorldAllocator;
}

namespace world::group
{
    class Group;

    class GroupBatch
    {
        friend class GroupBatchAllocator;

        container::Pool <Group *> groups; 

    public:
        void Add(Group *);

        void Remove(Group *);

        const container::Pool <Group *> &GetGroups() {return groups;}

        int GetGroupCount() const {return groups.GetSize();}
    };

    class GroupBatchAllocator
    {
        friend class world::WorldAllocator;

        static void AllocateBatch(GroupBatch &batch, container::PoolAllocator <group::Group *> &allocator)
        {
            batch.groups.Initialize(allocator);
        }
    };
};