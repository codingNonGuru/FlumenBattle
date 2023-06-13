#pragma once

#include "FlumenCore/Container/Pool.hpp"
#include "FlumenCore/Container/PoolAllocator.h"

#include "FlumenEngine/Core/Singleton.h"

namespace world
{
    struct Affliction;
    struct SettlementTile;
    struct SettlementEvent;
    class Settlement;
    class SettlementProduction;
    class SettlementFactory;
    class WorldTile;
    namespace group
    {
        class GroupDynamics;
    }
}

namespace world
{
    class SettlementAllocator : public Singleton<SettlementAllocator>
    {
        friend class WorldScene;

        friend class SettlementFactory;

        container::Pool <Settlement> settlements;

        container::Pool <group::GroupDynamics> groupDynamics;

        container::PoolAllocator <SettlementTile> tileAllocator;

        container::PoolAllocator <Affliction> afflictionAllocator;

        container::PoolAllocator <SettlementEvent> eventAllocator;

        container::Pool <SettlementProduction> productionAllocator;

        void PerformCleanup();

        Settlement *Allocate();

    public:
        SettlementAllocator();
    };
}