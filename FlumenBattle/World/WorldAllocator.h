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
        class GroupBatch;
        class Group;
    }

    class WorldAllocator : public core::Singleton <WorldAllocator>
    {
        typedef WorldTile* WorldTilePointer;

        friend class WorldGenerator;

        friend class WorldMap;

        friend class settlement::SettlementAllocator;

        bool hasPreallocatedMaximumMemory {false};

        container::Grid <WorldTile>::Memory worldTileMemory;

        container::Grid <SimulationDomain>::Memory simulationMemory;

        container::Grid <group::GroupBatch>::Memory batchMemory;

        container::PoolAllocator <group::Group *>::Memory groupBatchMemory;

        container::Array <WorldTile *>::Memory ownershipChangeMemory;


        container::PoolAllocator <group::Group *> groupBatchAllocator;

        void AllocateMap(WorldMap &/*, container::SmartBlock< container::Array <WorldTilePointer>, 4> &*/, int);

        void AllocateSociety(int);

    public:
        WorldAllocator();
    };
}