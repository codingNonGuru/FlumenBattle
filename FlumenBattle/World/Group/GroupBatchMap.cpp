#include "GroupBatchMap.h"
#include "FlumenBattle/World/Group/GroupBatch.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/Config.h"
#include "FlumenBattle/World/Group/Group.h"

using namespace world::group;

static const auto GROUPS_PER_VICINITY_BUFFER = 64;

static const auto GROUP_BUFFERS_PER_BATCH_MAP = 4;

static auto buffers = container::Pool <GroupBuffer> (GROUP_BUFFERS_PER_BATCH_MAP);

GroupBatch *GroupBatchMap::GetBatch(world::WorldTile *tile)
{
    static const auto GROUP_BATCH_TILE_SIZE = engine::ConfigManager::Get()->GetValue(game::ConfigValues::GROUP_BATCH_TILE_SIZE).Integer;

    if(tile == nullptr)
    {
        return nullptr;
    }

    auto x = tile->SquareCoordinates.x / GROUP_BATCH_TILE_SIZE;
    auto y = tile->SquareCoordinates.y / GROUP_BATCH_TILE_SIZE;

    return batches.Get(x, y);
}

const GroupBuffer GroupBatchMap::GetNearbyGroups(WorldTile *tile, int maximumGroupDistance) const
{
    static const auto GROUP_BATCH_TILE_SIZE = engine::ConfigManager::Get()->GetValue(game::ConfigValues::GROUP_BATCH_TILE_SIZE).Integer;

    auto newBuffer = buffers.Add();

    newBuffer->Groups.Reset();

    auto baseX = tile->SquareCoordinates.x / GROUP_BATCH_TILE_SIZE;
    auto baseY = tile->SquareCoordinates.y / GROUP_BATCH_TILE_SIZE;

    const auto range = maximumGroupDistance % GROUP_BATCH_TILE_SIZE == 0 ? maximumGroupDistance / GROUP_BATCH_TILE_SIZE : (maximumGroupDistance / GROUP_BATCH_TILE_SIZE) + 1;

    for(int i = -range; i <= range; ++i)
    {
        for(int j = -range; j <= range; ++j)
        {
            auto x = baseX + i;
            auto y = baseY + j;

            auto batch = batches.Get(x, y);

            auto &groups = batch->GetGroups();
            for(auto &group : groups)
            {
                auto distance = group->GetTile()->GetDistanceTo(*tile);
                if(distance > maximumGroupDistance)
                    continue;

                *newBuffer->Groups.Add() = group;
            }
        }   
    }

    return std::move(*newBuffer);
}

const GroupBuffer GroupBatchMap::GetGroupsInTile(WorldTile *tile) const
{
    static const auto GROUP_BATCH_TILE_SIZE = engine::ConfigManager::Get()->GetValue(game::ConfigValues::GROUP_BATCH_TILE_SIZE).Integer;

    auto newBuffer = buffers.Add();

    newBuffer->Groups.Reset();

    auto x = tile->SquareCoordinates.x / GROUP_BATCH_TILE_SIZE;
    auto y = tile->SquareCoordinates.y / GROUP_BATCH_TILE_SIZE;

    auto batch = batches.Get(x, y);

    auto &groups = batch->GetGroups();
    for(auto &group : groups)
    {
        if(group->GetTile() != tile)
            continue;

        *newBuffer->Groups.Add() = group;
    }

    return std::move(*newBuffer);
}

GroupBuffer::GroupBuffer() : Groups(GROUPS_PER_VICINITY_BUFFER) {}

GroupBuffer::~GroupBuffer()
{
    buffers.Remove(Groups.GetStart());
}