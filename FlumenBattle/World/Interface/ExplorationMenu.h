#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;
class LayoutGroup;

namespace world::interface
{
    class SpottingItem;

    class ExplorationMenu : public Element
    {
        Element *border;

        Text *reliefLabel;

        Text *ownerLabel;

        Text *tabLabel;

        LayoutGroup *spottingGroup;

        container::Array <SpottingItem *> spottingItems;

        void HandleConfigure() override;

        void HandleUpdate() override;

        void HandleSettlementEntered();

        void HandleSettlementExited();

        void HandleGroupSpotted();

        void HandleGroupFaded();
    };
}