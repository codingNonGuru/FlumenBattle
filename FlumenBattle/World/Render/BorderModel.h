#pragma once

#include "FlumenCore/Singleton.h"
#include "FlumenCore/Container/HexGrid.h"
#include "FlumenCore/Container/Block.hpp"

#include "FlumenEngine/Render/Model.hpp"

class Camera;

namespace world
{
    class WorldTile;
}

namespace world::render
{
    struct Border;
    
    struct BorderHex : public core::hex::Tile
    {
        struct NeighbourData
        {
            BorderHex *Neighbour;

            Border *Border {nullptr};
        };

        container::Block <NeighbourData, 6> Neighbours;

        WorldTile *Tile;

        bool HasBorderAt(BorderHex *hex)
        {
            for(auto &neighbour : Neighbours)
            {
                if(neighbour.Neighbour == hex && neighbour.Border != nullptr)
                {
                    return true;
                }
            }

            return false;
        }

        void AddBorder(BorderHex *hex, Border *border)
        {
            for(auto &neighbour : Neighbours)
            {
                if(neighbour.Neighbour == hex)
                {
                    neighbour.Border = border;
                    return;
                }
            }
        }
    };

    struct BorderData
    {
        Position2 Position;

        float Rotation;

        float Thickness;

        float Length;

        Float4 Color;

        bool IsInitialized {false};
    };

    struct Border : public core::hex::HexEdge <BorderHex>
    {
        BorderData FirstData;

        BorderData SecondData;

        Float Rotation;

        Border() {}

        Border(BorderHex *first, BorderHex *second) : core::hex::HexEdge <BorderHex> {first, second} {}// First(first), Second(second) {}
    };

    class BorderModel : public core::Singleton <BorderModel>, public Model
    {
        friend class core::Singleton <BorderModel>;

        container::EdgyHexGrid <BorderHex, Border> tiles;

        container::Grid <bool> edgeValidators;

        Camera *camera;

        void Update();

        void TransferData();

        void HandleWorldGenerated();

    public:
        void Initialize() override;

        void Render();
    };
}