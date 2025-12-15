#pragma once

#include "FlumenCore/Singleton.h"

#include "FlumenEngine/Container/HexGrid.h"

namespace world::tile
{
    struct WorldTile;
}

namespace world::render
{
    struct RoadRenderData
    {
        Float4 Color;

        Float2 Positions[3];

        float Thickness;

        float Padding[1];
    };

    struct PathTileData : core::hex::Tile
    {
        world::tile::WorldTile *Tile;

        Position2 Offset;    
    };

    class RoadModel : public core::Singleton <RoadModel>
    {
        friend class RendererAllocator;

        DataBuffer *buffer;

        DataBuffer *tileQueueBuffer;

        container::Array <RoadRenderData> data;

        container::Array <unsigned int> tileQueue;

        void UpdateData();

        container::HexGrid <PathTileData> &GetRoadTiles();

    public:
        void Initialize();

        void Render();
    };
}