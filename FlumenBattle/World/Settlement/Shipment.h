#pragma once

namespace world::settlement
{
    class Settlement;

    struct Shipment
    {
        Settlement *To {nullptr};
    };
}