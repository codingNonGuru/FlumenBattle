#pragma once

#include "FlumenCore/Conventions.hpp"
#include "FlumenCore/Container/Block.hpp"

#include "FlumenBattle/World/Tile/TileBuffer.h"

namespace world
{
    class WorldGenerator;

    class WorldAllocator;

    class WorldScene;
    
    class WorldController;
}

namespace world::tile
{
    struct WorldTile;

    template <typename TileType>
    struct TileBuffer;

    class WorldMap
    {
        friend class world::WorldScene;

        friend class world::WorldController;

        friend class WorldTileModel;

        friend class world::WorldGenerator;

        friend class world::WorldAllocator;

        friend class WorldInterface;

        Grid <WorldTile> tiles;

        WorldMap() {}

        WorldMap(Length size);

        WorldTile* GetRandomTile();

    public:
        static constexpr Float TILE_DISTANCING = 60.0f; 

        static constexpr Float WORLD_TILE_SIZE = 34.6666f;

        WorldTile* GetTile(Integer3) const;

        WorldTile* GetTile(Integer2) const;

        WorldTile* GetTile(Float2) const;

        WorldTile* GetRandomLandTile();

        WorldTile* GetEmptyRandomTile(bool = true);

        WorldTile* GetCenterTile();

        WorldTile* GetEmptyTileAroundTile(WorldTile *, Integer);

        const TileBuffer <WorldTile> GetNearbyTiles(WorldTile*, Integer);

        container::Block <WorldTile *, 6> GetNearbyTiles(WorldTile*);

        const TileBuffer <WorldTile> GetTileRing(WorldTile*, Integer);

        const int GetSize() const {return tiles.GetWidth();}

        const int GetTileCount() const {return tiles.GetWidth() * tiles.GetHeight();}

        const Grid <WorldTile> &GetTiles() const {return tiles;}
    };
}