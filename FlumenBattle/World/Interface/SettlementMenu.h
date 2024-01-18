#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

namespace world::settlement
{
    class Settlement;
}

namespace world::interface
{
    class SettlementMenu : public Element
    {
        Element *border;

        Text *nameLabel;

        settlement::Settlement *currentSettlement {nullptr};

        void HandleConfigure() override;

        void HandleUpdate() override;

    public:
        void Setup(settlement::Settlement *);
    };
}