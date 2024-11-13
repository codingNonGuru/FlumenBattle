#pragma once

#include "FlumenCore/Singleton.h"

#include "FlumenBattle/World/Group/GroupMind.h"

namespace world::group
{
    class MachineMind : public GroupMind, public core::Singleton <MachineMind>
    {
        void DetermineAction(Group &) const override;

        void DetermineActionAsMerchant(Group &) const;

        void DetermineActionAsAdventurer(Group &) const;

        void DetermineActionAsBandit(Group &) const;

        void DetermineActionAsPatrol(Group &) const;

        void DetermineActionAsGarrison(Group &) const;

        void DetermineActionAsRaider(Group &) const;

        void RegisterActionPerformance(Group &, GroupActionResult) const override;

        bool NeedsRest(const Group &) const;
    };
}