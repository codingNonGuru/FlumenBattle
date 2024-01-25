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

        container::Pool <GroupEssence> merchants;

        void Update(settlement::Settlement &);

        void StrengthenPatrol();

        void UpdateSimulationLevel(settlement::Settlement &);

        void AddAdventurer(settlement::Settlement &);

        void AddMerchant(settlement::Settlement &);

        void RemoveGroup(const group::Group &);

    public:
        GroupDynamics();

        void Initialize();

        int GetAdventurerStrength();

        int GetMerchantStrength();
    };
}