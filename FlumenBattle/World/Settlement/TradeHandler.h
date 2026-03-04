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

        int tradeBalance;

        Settlement *parent;

        void Initialize(Settlement *);

        void PrepareTransport(Settlement &);

        void SendTransport(Settlement &);

        void ReceiveTransport(Settlement &, ResourceData);

        void FinishUpdate(Settlement &);

        int GetTimeBetweenShipments(const Settlement &) const;

    public:
        static int GetRelationshipLevel(int);

        static float GetNextLevelProgress(int);

        static int GetMaximumTraffic();

        float GetProgress(const Settlement &) const;

        int GetBalance() const {return tradeBalance;}

        bool HasSevereTradeDeficit() const;
    };
}