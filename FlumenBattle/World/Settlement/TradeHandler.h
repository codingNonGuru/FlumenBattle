#pragma once

#include "FlumenBattle/World/Settlement/Shipment.h"

namespace world::settlement
{
    class Settlement;
    struct Shipment;

    struct ShipmentPriority
    {
        Settlement *To;

        int Degree;

        ResourceTypes Resource;
    };

    class TradeHandler
    {
        friend class Settlement;

        int lastShipmentTime;

        ShipmentPriority finalPriority;

        bool willShipThisTurn;

        Shipment lastOutgoingShipment;

        void Initialize() {lastShipmentTime = 0;}

        void PrepareTransport(Settlement &);

        void SendTransport(Settlement &);
    };
}