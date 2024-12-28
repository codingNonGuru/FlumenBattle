#pragma once

#include "FlumenCore/Container/Pool.hpp"

namespace world
{
    class WorldAllocator;
}

namespace world::group
{
    class GroupCore;

    class GroupBatch
    {
        friend class GroupBatchAllocator;

        container::Pool <GroupCore *> groups; 

    public:
        void Add(GroupCore *);

        void Remove(GroupCore *);

        const container::Pool <GroupCore *> &GetGroups() {return groups;}

        int GetGroupCount() const {return groups.GetSize();}
    };

    class GroupBatchAllocator
    {
        friend class world::WorldAllocator;

        static void AllocateBatch(GroupBatch &batch, container::PoolAllocator <group::GroupCore *> &allocator)
        {
            batch.groups.Initialize(allocator);
        }
    };
};