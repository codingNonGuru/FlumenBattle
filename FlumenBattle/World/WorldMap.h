#pragma once

#include "FlumenCore/Conventions.hpp"
#include "FlumenCore/Container/Block.hpp"

namespace world
{
    struct WorldTile;
    struct TileBuffer;

    struct TileBufferBatch
    {
        container::Pool <TileBuffer> Buffers;

        TileBufferBatch();
    };

    struct TileBuffer
    {
        container::Array <WorldTile *> Tiles;

        TileBufferBatch *Batch;

        TileBuffer();

        void Initialize(TileBufferBatch *);

        ~TileBuffer();

        TileBuffer(const TileBuffer &) = delete;

        TileBuffer& operator=(TileBuffer &buffer) = delete;

        TileBuffer(TileBuffer &&buffer) {Tiles = buffer.Tiles; Batch = buffer.Batch;}

        TileBuffer& operator=(TileBuffer &&buffer) {Tiles = buffer.Tiles; Batch = buffer.Batch;}

        bool operator== (WorldTile **tileStart) {return Tiles.GetStart() == tileStart;}

        int GetSize() const {return Tiles.GetSize();}

        auto begin() {return container::Array <WorldTile *>::Iterator <WorldTile *> {Tiles, Tiles.GetStart()};}

        auto end() {return container::Array <WorldTile *>::Iterator <WorldTile *> {Tiles, Tiles.GetEnd()};}
    };

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

        static constexpr Float WORLD_TILE_SIZE = 34.6666f;

        WorldTile* GetTile(Integer3) const;

        WorldTile* GetTile(Integer2) const;

        WorldTile* GetTile(Float2) const;

        WorldTile* GetRandomLandTile();

        WorldTile* GetEmptyRandomTile(bool = true);

        WorldTile* GetCenterTile();

        WorldTile* GetEmptyTileAroundTile(WorldTile *, Integer);

        const TileBuffer GetNearbyTiles(WorldTile*, Integer);

        const container::Block <WorldTile *, 6> GetNearbyTiles(WorldTile*);

        const TileBuffer GetTileRing(WorldTile*, Integer);

        const int GetSize() const {return tiles.GetWidth();}

        const int GetTileCount() const {return tiles.GetWidth() * tiles.GetHeight();}

        const Grid <WorldTile> &GetTiles() const {return tiles;}
    };
}