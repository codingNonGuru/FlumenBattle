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
    struct BorderHex;

    struct BorderData
    {
        Position2 Position;

        float Rotation;

        float Thickness;

        float Length;

        Float4 Color;

        bool IsInitialized {false};
    };

    struct Border
    {
        BorderHex *First;

        BorderHex *Second;

        BorderData FirstData;

        BorderData SecondData;

        Float Rotation;

        Border() {}

        Border(BorderHex *first, BorderHex *second) : First(first), Second(second) {}
    };

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

    class BorderModel : public core::Singleton <BorderModel>, public Model
    {
        friend class core::Singleton <BorderModel>;

        container::HexGrid <BorderHex> tiles;

        container::Array <Border> borders;

        Camera *camera;

        void Update();

        void TransferData();

    public:
        void Initialize() override;

        void Render();
    };
}