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
    enum class GroupClasses;

    class GroupDynamics
    {
        friend class world::settlement::Settlement;

        friend class world::settlement::SettlementLabel;

        friend class world::settlement::SettlementAllocator;

        int lastSpawnTime;

        container::Pool <GroupEssence> adventurers;

        container::Pool <GroupEssence> merchants;

        container::Pool <GroupEssence> bandits;

        container::Pool <GroupEssence> patrols;

        container::Pool <GroupEssence> garrisons;

        container::Pool <GroupEssence> raiders;

        int settlerCount;

        void Update(settlement::Settlement &);

        void UpdateEncounters(settlement::Settlement &);

        void UpdateSimulationLevel(settlement::Settlement &);

        void AddPatrol(settlement::Settlement &);

        void AddAdventurer(settlement::Settlement &);

        void AddMerchant(settlement::Settlement &);

        void AddBandit(settlement::Settlement &);

        void AddGarrison(settlement::Settlement &);

        void AddRaider(settlement::Settlement &);

        void AddSettlers(settlement::Settlement &);

        void RemoveSettlers() {settlerCount--;}

        void RemoveGroup(const group::Group &);

    public:
        GroupDynamics();

        void Initialize(settlement::Settlement &);

        int GetAdventurerStrength() const;

        int GetMerchantStrength() const;

        int GetPatrolStrength() const;

        int GetBanditStrength() const;

        int GetGarrisonStrength() const;

        int GetRaiderStrength() const;

        bool HasMaximumGarrisons() const;

        int GetMaximumGarrisons() const;

        int GetMaximumMerchants() const;

        int GetMaximumRaiders() const;

        int GetMaximumPatrols() const;

        int GetMaximumAdventurers() const;

        container::Array <Group *> &GetGroups(GroupClasses) const;

        bool HasMaximumSettlers() const;

        bool HasAnySettlers() const;
    };
}