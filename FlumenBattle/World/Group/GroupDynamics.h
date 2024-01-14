#pragma once

#include "FlumenCore/Conventions.hpp"

namespace world::settlement
{
    class Settlement;
    class SettlementLabel;
    class SettlementAllocator;
}

namespace world::group
{
    struct GroupEssence;
    class Group;

    class GroupDynamics
    {
        friend class world::settlement::Settlement;

        friend class world::settlement::SettlementLabel;

        friend class world::settlement::SettlementAllocator;

        int lastSpawnTime;

        Integer banditStrength;

        Integer patrolStrength;

        Integer merchantStrength;

        container::Pool <GroupEssence> adventurers;

        void Update(settlement::Settlement &);

        void StrengthenPatrol();

        void AddAdventurer(settlement::Settlement &);

        void RemoveGroup(const group::Group &);

    public:
        GroupDynamics();

        void Initialize();

        int GetAdventurerStrength();
    };
}