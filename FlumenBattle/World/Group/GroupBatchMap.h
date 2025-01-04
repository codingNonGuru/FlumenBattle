#pragma once

#include "FlumenCore/Container/Grid.hpp"
#include "FlumenCore/Container/Array.hpp"
#include "FlumenCore/Singleton.h"

namespace world
{
    class WorldAllocator;
    namespace tile {struct WorldTile;}
}

namespace world::group
{
    class GroupBatch;
    class GroupCore;

    struct GroupBuffer
    {
        container::Array <GroupCore *> Groups;

        GroupBuffer();

        ~GroupBuffer();

        GroupBuffer(const GroupBuffer &) = delete;

        GroupBuffer(GroupBuffer &&buffer) {Groups = buffer.Groups;}

        GroupBuffer& operator=(GroupBuffer &&buffer) {Groups = buffer.Groups;}

        bool operator== (GroupCore **groupStart) {return Groups.GetStart() == groupStart;}
    };

    class GroupBatchMap : public core::Singleton <GroupBatchMap>
    {
        friend class world::WorldAllocator;

        container::Grid <GroupBatch> batches;

    public:
        GroupBatch *GetBatch(tile::WorldTile *);

        const container::Grid <GroupBatch> &GetBatches() const {return batches;}

        const GroupBuffer GetNearbyGroups(tile::WorldTile *, int = 1) const;

        const GroupBuffer GetGroupsInTile(tile::WorldTile *) const;
    };
}