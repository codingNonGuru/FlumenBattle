#pragma once

#include "FlumenEngine/Interface/Element.hpp"

#include "FlumenBattle/World/Settlement/Types.h"

class Text;
class LayoutGroup;
class ProgressBar;

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
    class ResourceCounter;
}

namespace world::interface::rule
{
    class RuleMenu;
    class TechTab;

    struct TechItem : public Element
    {
        Text *iconLabel;

        TechTab *parentTab;

        const science::TechnologyType *technology;

        void Setup(const science::TechnologyType *, TechTab *);

        void HandleConfigure() override;

        void HandleUpdate() override {}
    };

    struct OptionItem : public Element
    {
        Element *icon;

        Text *nameLabel;

        ResourceCounter *counter;

        TechTab *parentTab;

        const science::TechnologyType *technology;

        void Setup(const science::TechnologyType *, TechTab *);

        void HandleConfigure() override;

        void HandleLeftClick() override;

        void HandleUpdate() override;
    };

    class TechTab : public Element
    {
        Text *headerLabel;

        Text *scienceLabel;

        ProgressBar *discoveryProgress;

        Text *discoveryLabel;

        container::Array <OptionItem *> optionItems;

        container::Array <TechItem *> techItems;

        LayoutGroup *optionLayout;

        LayoutGroup *techLayout;

        void HandleConfigure() override;

        void HandleEnable() override;

        void HandleUpdate() override;

        void HandleSettlementChanged();

    public:
        //TechHoverInfo *GetHoverDevice() {return techHoverInfo;}
    };
}