#pragma once

#include "FlumenCore/Conventions.hpp"

namespace world
{
    class Settlement;
    class SettlementLabel;
}

namespace world::group
{
    class GroupDynamics
    {
        friend class world::Settlement;

        friend class world::SettlementLabel;

        Integer banditStrength;

        Integer patrolStrength;

        Integer merchantStrength;

        void Update(Settlement &);

        void StrengthenPatrol();

    public:
        GroupDynamics();
    };
}