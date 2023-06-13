#pragma once

#include "FlumenBattle/World/Types.h"

namespace world::settlement
{
    class Settlement;
    struct AfflictionResult;

    struct SettlementEvent
    {
        SettlementEventTypes EventType;

        AfflictionTypes CauseType;
    };

    class SettlementEventGenerator
    {
        friend class Settlement;

        static void GenerateEvent(Settlement &, const AfflictionResult &);
    };
}