#pragma once

#include "FlumenEngine/Interface/Element.hpp"

#include "FlumenBattle/World/Settlement/Types.h"

class Text;
class LayoutGroup;
class ProgressBar;

namespace world::settlement
{
    class Settlement;
    struct Resource;
}

namespace world::interface
{
    class Counter;
}

namespace world::interface::rule
{
    class RuleMenu;
    class ProductionDecisionInterface;

    class RecruitmentTab : public Element
    {
        Text *nameLabel;

        ProductionDecisionInterface *decisionInterface;

        void HandleConfigure() override;

        void HandleUpdate() override;

        void HandleEnable() {}

        void HandleSettlementChanged();
    };
}