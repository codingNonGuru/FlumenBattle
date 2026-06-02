#pragma once

#include "FlumenBattle/World/Tile/WorldTile.h"

namespace world::tile
{
    class River;

    struct WorldEdge : public core::hex::HexEdge <WorldTile>
    {
        River *River;

        WorldTile *Start;

        WorldTile *End;

        int HeightDelta;

        int Discharge;

        WorldTile *Downstream;

        WorldTile *GetUpstream() {return (Start == Downstream ? End : Start);}
    };

    struct WorldCorner : public core::hex::HexCorner <WorldTile>
    {
        Position2 Position;

        bool IsFullyLand;

        bool IsFullySea;

        bool IsFullySteppe;

        bool IsNotAridDesert;
    };
}