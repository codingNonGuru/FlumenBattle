#pragma once

#include "FlumenEngine/Interface/Element.hpp"

#include "FlumenBattle/World/Settlement/Types.h"

class Text;
class LayoutGroup;

namespace world::settlement
{
    class Settlement;
}

namespace world::science
{
    class TechnologyType;
}

namespace world::interface
{
    class Counter;
}

namespace world::interface::rule
{
    class RuleMenu;
    class TechTab;

    struct OptionItem : public Element
    {
        Element *icon;

        Text *nameLabel;

        Counter *counter;

        TechTab *parentTab;

        void Setup(const science::TechnologyType *, TechTab *);

        void HandleConfigure() override;

        void HandleLeftClick() override {}
    };

    class TechTab : public Element
    {
        Text *headerLabel;

        Text *scienceLabel;

        container::Array <OptionItem *> optionItems;

        //container::Array <BuildingItem *> buildingItems;

        LayoutGroup *optionLayout;

        void HandleConfigure() override;

        void HandleEnable() override;

        void HandleUpdate() override;

        void HandleSettlementChanged();

    public:
        //TechHoverInfo *GetHoverDevice() {return techHoverInfo;}
    };
}