#pragma once

#include "FlumenCore/Container/Grid.hpp"
#include "FlumenCore/Singleton.h"

namespace world
{
    struct SimulationDomain;
    namespace tile {class WorldMap;}
    namespace tile {struct WorldTile;}

    class SimulationMap : public core::Singleton <SimulationMap>
    {
        friend class WorldAllocator;

        container::Grid <SimulationDomain> domains;

    public:
        void Initialize();

        void Update();

        SimulationDomain *GetDomain(int, int);

        SimulationDomain *GetDomain(tile::WorldTile *);
    };
}