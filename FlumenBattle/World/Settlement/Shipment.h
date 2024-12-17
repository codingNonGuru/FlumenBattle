#pragma once

#include "FlumenBattle/World/Settlement/Types.h"

namespace world::settlement
{
    class Settlement;

    struct Shipment
    {
        Settlement *From;

        Settlement *To;

        ResourceTypes Resource;

        int AmountSent;

        int AmountReceived;

        int TimeInAbsoluteTicks;
    };
}