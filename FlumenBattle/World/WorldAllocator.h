#pragma once

#include "FlumenCore/Container/Array.hpp"
#include "FlumenCore/Container/Pool.hpp"
#include "FlumenCore/Container/Grid.hpp"

#include "FlumenEngine/Core/Singleton.h"

namespace world
{
    class WorldScene;
    class WorldTile;
    class WorldMap;
    class Polity;

    namespace settlement
    {
        class Settlement;
        class SettlementAllocator;
    }

    namespace group
    {
        class GroupDynamics;
    }

    class WorldAllocator : public Singleton <WorldAllocator>
    {
        typedef WorldTile* WorldTilePointer;

        friend class WorldGenerator;

        friend class WorldMap;

        friend class settlement::SettlementAllocator;

        bool hasPreallocatedMaximumMemory {false};

        container::Array <WorldTilePointer>::Memory nearbyTileMemory;

        container::Grid <WorldTile>::Memory worldTileMemory;

        container::Pool <Polity>::Memory polityMemory;

        void AllocateMap(WorldMap &, container::Array <WorldTilePointer> &, int);

        void AllocateSociety(int);

    public:
        WorldAllocator();
    };
}