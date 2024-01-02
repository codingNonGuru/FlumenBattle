#pragma once

#include "FlumenBattle/World/Types.h"

namespace world::settlement
{
    class Settlement;
    struct AfflictionResult;

    struct Event
    {
        EventTypes EventType;

        AfflictionTypes CauseType;

        int Time;
    };

    class EventGenerator
    {
        friend class Settlement;

        static void GenerateEvent(Settlement &, const AfflictionResult &);
    };
}