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

        void FinishUpdate(Settlement &);

        int GetTimeBetweenShipments(const Settlement &) const;

    public:
        static int GetRelationshipLevel(int);

        static float GetNextLevelProgress(int);

        static int GetMaximumTraffic();

        float GetProgress(const Settlement &) const;
    };
}