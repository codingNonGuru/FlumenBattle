#pragma once

#include "FlumenCore/Conventions.hpp"
#include "FlumenCore/Container/Block.hpp"

namespace world
{
    struct WorldTile;

    class WorldMap
    {
        friend class WorldScene;

        friend class WorldController;

        friend class WorldTileModel;

        friend class WorldGenerator;

        friend class WorldAllocator;

        friend class WorldInterface;

        Grid <WorldTile> tiles;

        WorldMap() {}

        WorldMap(Length size);

        WorldTile* GetRandomTile();

    public:
        static constexpr Float TILE_DISTANCING = 60.0f; 

        WorldTile* GetTile(Integer3);

        WorldTile* GetRandomLandTile();

        WorldTile* GetEmptyRandomTile(bool = true);

        WorldTile* GetCenterTile();

        WorldTile* GetEmptyTileAroundTile(WorldTile *, Integer);

        const Array<WorldTile*> & GetNearbyTiles(WorldTile*, Integer, int = 0);

        const container::Block <WorldTile *, 6> GetNearbyTiles(WorldTile*);

        const Array<WorldTile*> & GetTileRing(WorldTile*, Integer);
    };
}