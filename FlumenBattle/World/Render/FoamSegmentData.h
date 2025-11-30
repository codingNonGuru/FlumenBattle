#pragma once

namespace world::tile
{
    struct WorldTile;
}

namespace world::render
{
    struct FoamSegmentData
    {
        world::tile::WorldTile *First;

        world::tile::WorldTile *Second;
    };
}