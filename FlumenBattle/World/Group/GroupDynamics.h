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

        Integer patrolStrength;

        container::Pool <GroupEssence> adventurers;

        container::Pool <GroupEssence> merchants;

        container::Pool <GroupEssence> bandits;

        void Update(settlement::Settlement &);

        void UpdateEncounters(settlement::Settlement &);

        void StrengthenPatrol();

        void UpdateSimulationLevel(settlement::Settlement &);

        void AddAdventurer(settlement::Settlement &);

        void AddMerchant(settlement::Settlement &);

        void AddBandit(settlement::Settlement &);

        void RemoveGroup(const group::Group &);

    public:
        GroupDynamics();

        void Initialize();

        int GetAdventurerStrength();

        int GetMerchantStrength();

        int GetPatrolStrength() const {return patrolStrength;}

        int GetBanditStrength() const;
    };
}