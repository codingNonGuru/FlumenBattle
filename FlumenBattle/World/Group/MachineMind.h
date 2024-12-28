#pragma once

#include "FlumenCore/Singleton.h"

#include "FlumenBattle/World/Group/GroupMind.h"

namespace world::group
{
    class MachineMind : public GroupMind, public core::Singleton <MachineMind>
    {
        friend class GroupCore;

        void DetermineAction(GroupCore &) const override;

        void DetermineActionAsMerchant(GroupCore &) const;

        void DetermineActionAsAdventurer(GroupCore &) const;

        void DetermineActionAsBandit(GroupCore &) const;

        void DetermineActionAsPatrol(GroupCore &) const;

        void DetermineActionAsGarrison(GroupCore &) const;

        void DetermineActionAsRaider(GroupCore &) const;

        void RegisterActionPerformance(GroupCore &, GroupActionResult) const override;

        bool NeedsRest(const GroupCore &) const;
    };
}