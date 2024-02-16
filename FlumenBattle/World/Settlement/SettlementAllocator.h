#pragma once

#include "FlumenCore/Container/Pool.hpp"
#include "FlumenCore/Container/PoolAllocator.h"

#include "FlumenCore/Singleton.h"

namespace world
{
    class WorldTile;
    class WorldScene;
    class WorldAllocator;
    namespace group
    {
        class GroupDynamics;
        struct GroupEssence;
    }
}

namespace world::settlement
{
    struct Affliction;
    struct SettlementTile;
    struct Event;
    class Settlement;
    class SettlementProduction;
    class SettlementFactory;
    struct Path;
    struct Link;
    class PathSegment;
    class Condition;
    class ConditionManager;
    class Building;
    class BuildingManager;
    struct Modifier;
    struct Resource;

    class SettlementAllocator : public core::Singleton <SettlementAllocator>
    {
        friend class world::WorldScene;

        friend class SettlementFactory;

        friend class world::WorldAllocator;

        friend class Path;
        
        friend struct Link;

        container::Pool <Settlement>::Memory settlementMemory;

        container::Pool <group::GroupDynamics>::Memory groupDynamicsMemory;

        container::PoolAllocator <SettlementTile>::Memory tileMemory;

        container::PoolAllocator <Affliction>::Memory afflictionMemory;

        container::PoolAllocator <Event>::Memory eventMemory;

        container::Pool <SettlementProduction>::Memory productionMemory;

        container::Pool <ConditionManager>::Memory conditionManagerMemory;

        container::PoolAllocator <Condition>::Memory conditionMemory;

        container::Pool <BuildingManager>::Memory buildingManagerMemory;

        container::PoolAllocator <Building>::Memory buildingMemory;

        container::Pool <Path>::Memory pathMemory;

        container::Pool <PathSegment>::Memory pathSegmentMemory;

        container::PoolAllocator <Link>::Memory linkMemory;

        container::ArrayAllocator <Modifier>::Memory modifierMemory;

        container::ArrayAllocator <Resource>::Memory resourceMemory;

        container::PoolAllocator <group::GroupEssence>::Memory adventurerMemory;

        container::PoolAllocator <group::GroupEssence>::Memory merchantMemory;

        container::PoolAllocator <group::GroupEssence>::Memory banditMemory;


        container::Pool <Settlement> settlements;

        container::Pool <group::GroupDynamics> groupDynamics;

        container::PoolAllocator <SettlementTile> tileAllocator;

        container::PoolAllocator <Affliction> afflictionAllocator;

        container::PoolAllocator <Event> eventAllocator;

        container::Pool <SettlementProduction> productionAllocator;

        container::Pool <ConditionManager> conditionManagerAllocator;

        container::PoolAllocator <Condition> conditionAllocator;

        container::Pool <BuildingManager> buildingManagerAllocator;

        container::PoolAllocator <Building> buildingAllocator;

        container::Pool <Path> paths;

        container::Pool <PathSegment> pathSegments;

        container::PoolAllocator <Link> linkAllocator;

        container::ArrayAllocator <Modifier> modifierAllocator;

        container::ArrayAllocator <Resource> resourceAllocator;

        container::PoolAllocator <group::GroupEssence> adventurerAllocator;

        container::PoolAllocator <group::GroupEssence> merchantAllocator;

        container::PoolAllocator <group::GroupEssence> banditAllocator;


        void PerformCleanup();

        Settlement *Allocate();

        Path *AllocatePath(Settlement *, Settlement *);

        PathSegment *AllocateSegment();

        void AllocateWorldMemory(int);

        void PreallocateMaximumMemory();

    public:
        container::Pool <Settlement> *GetSettlements() {return &settlements;}

        container::Pool <Path> *GetPaths() {return &paths;}

        container::Pool <PathSegment> *GetPathSegments() {return &pathSegments;}
    };
}