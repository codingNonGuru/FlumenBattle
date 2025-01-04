#pragma once

#include "FlumenCore/Container/Array.hpp"
#include "FlumenCore/Container/Pool.hpp"
#include "FlumenCore/Container/Grid.hpp"
#include "FlumenCore/Container/SmartBlock.hpp"

#include "FlumenCore/Singleton.h"

namespace world
{
    class WorldScene;
    namespace tile 
    {
        struct WorldTile;
        class WorldMap;
        class Ruin;
        class RuinHandler;
    }

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
        class GroupCore;
    }

    class WorldAllocator : public core::Singleton <WorldAllocator>
    {
        typedef tile::WorldTile* WorldTilePointer;

        friend class WorldGenerator;

        friend class tile::WorldMap;

        friend class settlement::SettlementAllocator;

        bool hasPreallocatedMaximumMemory {false};

        container::Grid <tile::WorldTile>::Memory worldTileMemory;

        container::Grid <SimulationDomain>::Memory simulationMemory;

        container::Grid <group::GroupBatch>::Memory batchMemory;

        container::PoolAllocator <group::GroupCore *>::Memory groupBatchMemory;

        container::Array <tile::WorldTile *>::Memory ownershipChangeMemory;

        container::ArrayAllocator <tile::Ruin>::Memory ruinMemory;

        container::Array <tile::RuinHandler>::Memory ruinHandlerMemory;


        container::PoolAllocator <group::GroupCore *> groupBatchAllocator;

        container::ArrayAllocator <tile::Ruin> ruinAllocator;

        container::Array <tile::RuinHandler> ruinHandlerAllocator;

        void AllocateMap(tile::WorldMap &/*, container::SmartBlock< container::Array <WorldTilePointer>, 4> &*/, int);

        void AllocateSociety(int);

    public:
        WorldAllocator();

        tile::RuinHandler *AllocateRuinHandler();
    };
}