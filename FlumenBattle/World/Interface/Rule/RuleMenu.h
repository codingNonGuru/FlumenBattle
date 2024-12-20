#pragma once

#include "FlumenCore/Observer.h"

#include "FlumenEngine/Interface/Element.hpp"

#include "FlumenBattle/World/Settlement/Types.h"

class Text;
class LayoutGroup;

namespace world::settlement
{
    class Settlement;
    struct Resource;
    struct Building;
}

namespace world::interface
{
    class Counter;
    class BuildingHoverInfo;
}

namespace world::interface::rule
{
    enum class RuleMenuTabs
    {
        REALM, ECONOMY, TECHNOLOGY, BUILDING, TRADE, GROUPS, //DIPLOMACY, SETTLEMENT, POPULATION
        COUNT
    };

    class TabButton : public Element
    {
        Text *label;

        void HandleConfigure() override;

    public:
        void Setup(RuleMenuTabs);

        void Light();

        void Fade();
    };

    class RuleMenu : public Element
    {
        Text *rulerLabel;

        Element *border;

        settlement::Settlement *settlement {nullptr};

        RuleMenuTabs currentTab;

        container::StaticMap <Element *, RuleMenuTabs> tabs;

        LayoutGroup *buttonLayout;

        container::StaticMap <TabButton *, RuleMenuTabs> tabButtons;

        void HandleConfigure() override;

        void HandleEnable() override;

        void HandleDisable() override;

        void HandleEconomyPressed();

        void HandleTechnologyPressed();

        void HandleRealmPressed();

        void HandleBuildingPressed();

        void HandleTradePressed();

        void HandleGroupsPressed();

        void SetCurrentTab(RuleMenuTabs);

    public:
        Delegate OnSettlementChanged;

        void Setup();

        void SetCurrentSettlement(settlement::Settlement *);

        settlement::Settlement *GetCurrentSettlement() {return settlement;}
    };
}