#pragma once

#include "FlumenEngine/Core/Singleton.h"

#include "FlumenBattle/World/Group/GroupMind.h"

namespace world::group
{
    class MachineMind : public GroupMind, public Singleton <MachineMind>
    {
        void DetermineAction(Group &) const override;

        void RegisterActionPerformance(Group &, GroupActionResult) const override;
    };
}