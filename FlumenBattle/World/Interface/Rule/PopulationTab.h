#pragma once

#include "FlumenEngine/Interface/Element.hpp"

#include "FlumenBattle/World/Settlement/Types.h"

class Text;
class LayoutGroup;
class ProgressBar;
class SimpleList;

namespace world::settlement
{
    class Settlement;
    struct Resource;
}

namespace world::interface
{
    class Counter;
    class CounterSet;
}

namespace world::interface::rule
{
    class RuleMenu;
    //class ProductionDecisionInterface;
    class PopulationItem;

    class PopulationTab : public Element
    {
        Text *nameLabel;

        //ProductionDecisionInterface *decisionInterface;

        container::Array <PopulationItem *> popItems;

        LayoutGroup *popItemLayout;

        CounterSet *counterSet;

        void UpdateItems();

        void HandleConfigure() override;

        //void HandleUpdate() override;

        void HandleEnable() override;

        void HandleSettlementChanged();
    };
}