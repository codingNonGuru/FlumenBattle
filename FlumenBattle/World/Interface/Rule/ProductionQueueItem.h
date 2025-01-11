#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

namespace world::polity
{
    struct ProductionQueueSlot;
}

namespace world::interface::rule
{
    class ProductionQueueItem : public Element
    {
        Text *label;

        polity::ProductionQueueSlot *queueSlot;

        void HandleConfigure() override;

        void HandleUpdate() override;

        void HandleLeftClick() override;

    public:
        void Setup(polity::ProductionQueueSlot *);
    };
}