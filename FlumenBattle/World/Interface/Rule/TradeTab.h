#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;
class SimpleList;
class ProgressBar;

namespace world::settlement
{
    class Settlement;
    class Shipment;
    struct Link;
}

namespace world::interface::rule
{
    class LinkItem : public Element
    {
        Text *settlementLabel;

        ProgressBar *relationshipBar;

        Text *levelLabel;

        void HandleConfigure() override;

    public:
        void Setup(const settlement::Link *);
    };

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
        SimpleList *tradeItemList;

        container::Array <TradeItem *> tradeItems;

        SimpleList *linkItemList;

        container::Array <LinkItem *> linkItems;

        settlement::Settlement *settlement;

        void HandleConfigure() override;

        void HandleUpdate() override;

        void HandleSettlementChanged();

        void HandlePlayerShipment();
    };
}