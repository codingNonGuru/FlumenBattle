#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;
class SimpleList;

namespace world::settlement
{
    class Settlement;
    class Shipment;
}

namespace world::interface::rule
{
    class TradeItem : public Element
    {
        Element *icon;

        Text *settlementLabel;

        Text *volumeLabel;

        Text *timeLabel;

        const settlement::Shipment *shipment;

        void HandleConfigure() override;

        void HandleUpdate() override;

    public:
        void Setup(const settlement::Shipment &, settlement::Settlement *);
    };

    class TradeTab : public Element
    {
        SimpleList *itemList;

        container::Array <TradeItem *> items;

        settlement::Settlement *settlement;

        void HandleConfigure() override;

        void HandleUpdate() override;

        void HandleSettlementChanged();

        void HandlePlayerShipment();
    };
}