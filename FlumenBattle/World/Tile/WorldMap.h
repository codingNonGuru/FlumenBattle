#pragma once

#include "FlumenCore/Conventions.hpp"
#include "FlumenCore/Container/Block.hpp"
#include "FlumenCore/Hex/Hex.h"

#include "FlumenEngine/Container/HexGrid.h"
#include "FlumenEngine/Thread/HexBuffer.h"

#include "FlumenBattle/World/Tile/WorldTile.h"

namespace world
{
    class WorldGenerator;

    class WorldAllocator;

    class WorldScene;
    
    class WorldController;
}

namespace world::tile
{
    class WorldMap : public container::HexGrid <WorldTile>
    {
        friend class world::WorldScene;

        friend class world::WorldController;

        friend class WorldTileModel;

        friend class world::WorldGenerator;

        friend class world::WorldAllocator;

        friend class WorldInterface;

        WorldMap() {}

        WorldMap(Length size);

    public:
        static constexpr Float TILE_DISTANCING = 60.0f; 

        static constexpr Float WORLD_TILE_SIZE = 34.6666f;

        WorldTile* GetTileFromPosition(Float2) const;

        WorldTile* GetRandomLandTile();

        WorldTile* GetEmptyRandomTile(bool = true);

        WorldTile* GetEmptyTileAroundTile(WorldTile *, Integer);
    };
}