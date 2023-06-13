#pragma once

#include "FlumenCore/Conventions.hpp"

namespace world::settlement
{
    class Settlement;
    class SettlementLabel;
}

namespace world::group
{
    class GroupDynamics
    {
        friend class world::settlement::Settlement;

        friend class world::settlement::SettlementLabel;

        Integer banditStrength;

        Integer patrolStrength;

        Integer merchantStrength;

        void Update(settlement::Settlement &);

        void StrengthenPatrol();

    public:
        GroupDynamics();
    };
}