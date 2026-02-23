#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

namespace world::settlement
{
    class Settlement;
}

namespace world::interface
{
    class CounterSet;
}

namespace world::interface::rule
{
    class DomainItem;

    class DomainHoverInfo : public Element
    {
        Element *border;

        Text *nameLabel;

        CounterSet *resourceCounterSet;

        DomainItem *hoveredItem {nullptr};

        const settlement::Settlement *settlement;

        void HandleConfigure() override;

        void HandleUpdate() override;

    public:
        void Setup(DomainItem *);
    };
}