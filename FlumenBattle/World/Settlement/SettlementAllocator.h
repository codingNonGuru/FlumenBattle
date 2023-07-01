#pragma once

#include "FlumenCore/Container/Pool.hpp"
#include "FlumenCore/Container/PoolAllocator.h"

#include "FlumenEngine/Core/Singleton.h"

namespace world
{
    class WorldTile;
    class WorldScene;
    class WorldAllocator;
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

        friend class world::WorldAllocator;

        container::Pool <Settlement>::Memory settlementMemory;

        container::Pool <group::GroupDynamics>::Memory groupDynamicsMemory;

        container::PoolAllocator <SettlementTile>::Memory tileMemory;

        container::PoolAllocator <Affliction>::Memory afflictionMemory;

        container::PoolAllocator <SettlementEvent>::Memory eventMemory;

        container::Pool <SettlementProduction>::Memory productionMemory;

        container::Pool <ConditionManager>::Memory conditionManagerMemory;

        container::PoolAllocator <Condition>::Memory conditionMemory;

        container::Pool <BuildingManager>::Memory buildingManagerMemory;

        container::PoolAllocator <Building>::Memory buildingMemory;

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

        void AllocateWorldMemory(int);

        void PreallocateMaximumMemory();

    public:
        container::Pool <Settlement> *GetSettlements() {return &settlements;}
    };
}