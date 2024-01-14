#pragma once

#include "FlumenCore/Container/Grid.hpp"
#include "FlumenCore/Singleton.h"

namespace world
{
    struct SimulationDomain;
    class WorldMap;
    class WorldTile;

    class SimulationMap : public core::Singleton <SimulationMap>
    {
        friend class WorldAllocator;

        container::Grid <SimulationDomain> domains;

    public:
        void Initialize();

        void Update();

        SimulationDomain *GetDomain(int, int);

        SimulationDomain *GetDomain(WorldTile *);
    };
}