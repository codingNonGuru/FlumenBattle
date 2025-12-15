#pragma once

#include "FlumenCore/Singleton.h"

namespace world
{
    namespace tile
    {
        struct WorldEdge;
    }

    class RiverGenerator : public core::Singleton <RiverGenerator>
    {
        int GetNeighbouringSeaTiles(tile::WorldEdge &);

        void ComputeSlopes();

        void EstablishRiverMouths();

        void CarveUpstream();

        void ComputeDischarge();

        void EstablishCorners();

    public:
        void DefineRivers();
    };
}