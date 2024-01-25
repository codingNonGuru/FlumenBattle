#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

namespace world::interface
{
    class ExplorationMenu : public Element
    {
        Element *border;

        Text *reliefLabel;

        Text *ownerLabel;

        Text *tabLabel;

        void HandleConfigure() override;

        void HandleUpdate() override;

        void HandleSettlementEntered();

        void HandleSettlementExited();
    };
}