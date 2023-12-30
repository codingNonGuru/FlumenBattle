#pragma once

#include "FlumenCore/Singleton.h"

#include "FlumenBattle/World/Group/GroupMind.h"

namespace world::group
{
    class MachineMind : public GroupMind, public core::Singleton <MachineMind>
    {
        void DetermineAction(Group &) const override;

        void RegisterActionPerformance(Group &, GroupActionResult) const override;
    };
}