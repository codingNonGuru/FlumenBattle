#pragma once

#include "FlumenCore/Container/Pool.hpp"
#include "FlumenCore/Container/PoolAllocator.h"

#include "FlumenEngine/Core/Singleton.h"

namespace world
{
    class WorldTile;
    class WorldScene;
    namespace group
    {
        class GroupDynamics;
    }
}

namespace world::settlement
{
    struct Affliction;
    struct SettlementTile;
    struct SettlementEvent;
    class Settlement;
    class SettlementProduction;
    class SettlementFactory;
    class Condition;
    class ConditionManager;
    class Building;
    class BuildingManager;

    class SettlementAllocator : public Singleton<SettlementAllocator>
    {
        friend class world::WorldScene;

        friend class SettlementFactory;

        container::Pool <Settlement> settlements;

        container::Pool <group::GroupDynamics> groupDynamics;

        container::PoolAllocator <SettlementTile> tileAllocator;

        container::PoolAllocator <Affliction> afflictionAllocator;

        container::PoolAllocator <SettlementEvent> eventAllocator;

        container::Pool <SettlementProduction> productionAllocator;

        container::Pool <ConditionManager> conditionManagerAllocator;

        container::PoolAllocator <Condition> conditionAllocator;

        container::Pool <BuildingManager> buildingManagerAllocator;

        container::PoolAllocator <Building> buildingAllocator;

        void PerformCleanup();

        Settlement *Allocate();

    public:
        SettlementAllocator();
    };
}