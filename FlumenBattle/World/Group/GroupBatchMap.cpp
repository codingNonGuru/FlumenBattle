#include "GroupBatchMap.h"
#include "FlumenBattle/World/Group/GroupBatch.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/Config.h"

using namespace world::group;

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