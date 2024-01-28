#pragma once

#include "FlumenCore/Container/Grid.hpp"
#include "FlumenCore/Container/Array.hpp"
#include "FlumenCore/Singleton.h"

namespace world
{
    class WorldAllocator;
    class WorldTile;
}

namespace world::group
{
    class GroupBatch;
    class Group;

    struct GroupBuffer
    {
        container::Array <Group *> Groups;

        GroupBuffer();

        ~GroupBuffer();

        GroupBuffer(const GroupBuffer &) = delete;

        GroupBuffer(GroupBuffer &&buffer) {Groups = buffer.Groups;}

        GroupBuffer& operator=(GroupBuffer &&buffer) {Groups = buffer.Groups;}

        bool operator== (Group **groupStart) {return Groups.GetStart() == groupStart;}
    };

    class GroupBatchMap : public core::Singleton <GroupBatchMap>
    {
        friend class world::WorldAllocator;

        container::Grid <GroupBatch> batches;

    public:
        GroupBatch *GetBatch(world::WorldTile *);

        const container::Grid <GroupBatch> &GetBatches() const {return batches;}

        const GroupBuffer GetNearbyGroups(WorldTile *, int = 1) const;
    };
}