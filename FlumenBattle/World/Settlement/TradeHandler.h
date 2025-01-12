#pragma once

#include "FlumenBattle/World/Settlement/Shipment.h"

namespace world::interface::rule
{
    class TradeTab;
}

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

        friend class interface::rule::TradeTab;

        int lastShipmentTime;

        ShipmentPriority finalPriority;

        bool willShipThisTurn;

        Shipment lastOutgoingShipment;

        int dailyShipmentCount;

        int weeklyShipmentCount;

        int monthlyShipmentCount;

        int yearlyShipmentCount;

        int totalShipmentCount;

        void Initialize();

        void PrepareTransport(Settlement &);

        void SendTransport(Settlement &);

        void ReceiveTransport(Settlement &);

        void FinishUpdate(Settlement &);

        int GetTimeBetweenShipments(const Settlement &) const;

    public:
        static int GetRelationshipLevel(int);

        static float GetNextLevelProgress(int);

        static int GetMaximumTraffic();

        float GetProgress(const Settlement &) const;
    };
}