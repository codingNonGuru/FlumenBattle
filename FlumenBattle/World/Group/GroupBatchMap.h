#pragma once

#include "FlumenCore/Container/Grid.hpp"
#include "FlumenCore/Singleton.h"

namespace world
{
    class WorldAllocator;
    class WorldTile;
}

namespace world::group
{
    class GroupBatch;

    class GroupBatchMap : public core::Singleton <GroupBatchMap>
    {
        friend class world::WorldAllocator;

        container::Grid <GroupBatch> batches;

    public:
        GroupBatch *GetBatch(world::WorldTile *);

        const container::Grid <GroupBatch> &GetBatches() const {return batches;}
    };
}