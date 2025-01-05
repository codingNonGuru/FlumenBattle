#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;
class SimpleList;
class ProgressBar;

namespace world::interface
{
    class Counter;
}

namespace world::interface::rule
{
    class RuleMenu;
    class NeighborItem;

    class DiplomacyTab : public Element
    {
        Text *nameLabel;

        container::Array <NeighborItem *> neighborItems;

        SimpleList *neighborItemLayout;

        void HandleConfigure() override;

        void HandleUpdate() override;

        void HandleEnable() override {}

        void HandleSettlementChanged();
    };
}