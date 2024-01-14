#pragma once

#include "FlumenCore/Container/Array.hpp"
#include "FlumenCore/Container/Pool.hpp"
#include "FlumenCore/Container/Grid.hpp"
#include "FlumenCore/Container/SmartBlock.hpp"

#include "FlumenCore/Singleton.h"

namespace world
{
    class WorldScene;
    class WorldTile;
    class WorldMap;
    class Polity;
    struct SimulationDomain;

    namespace settlement
    {
        class Settlement;
        class SettlementAllocator;
    }

    namespace group
    {
        class GroupDynamics;
    }

    class WorldAllocator : public core::Singleton <WorldAllocator>
    {
        typedef WorldTile* WorldTilePointer;

        friend class WorldGenerator;

        friend class WorldMap;

        friend class settlement::SettlementAllocator;

        bool hasPreallocatedMaximumMemory {false};

        container::Array <WorldTilePointer>::Memory nearbyTileMemories[4];

        container::Grid <WorldTile>::Memory worldTileMemory;

        container::Grid <SimulationDomain>::Memory simulationMemory;

        void AllocateMap(WorldMap &, container::SmartBlock< container::Array <WorldTilePointer>, 4> &, int);

        void AllocateSociety(int);

    public:
        WorldAllocator();
    };
}