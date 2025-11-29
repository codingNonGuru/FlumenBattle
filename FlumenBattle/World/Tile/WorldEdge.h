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
    };
}