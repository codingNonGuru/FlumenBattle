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
    struct Building;
}

namespace world::interface
{
    class Counter;
}

namespace world::interface::rule
{
    class RuleMenu;
    class BuildingTab;
    class BuildingHoverInfo;

    struct BuildingItem : public Element
    {
        friend class RuleMenu;

        //Text *nameLabel;

        Element *icon;

        Counter *counter;

        Counter *workerCounter;

        int buildingCount;

        int workerCount;

        settlement::Building *building;

        BuildingTab *parentTab;

        void Setup(settlement::Building *, BuildingTab *);

        void HandleConfigure() override;

        void HandleUpdate() override;

        void HandleHover() override;

        void HandleLeftClick() override;

        void HandleRightClick() override;

    public:
        const settlement::Building *GetBuilding() const {return building;}
    };

    class BuildingTab : public Element
    {
        Text *nameLabel;

        container::Array <BuildingItem *> buildingItems;

        LayoutGroup *buildingLayout;

        BuildingHoverInfo *buildingHoverInfo;

        void HandleConfigure() override;

        void HandleUpdate() override;

        void HandleEnable() {}

        void HandleSettlementChanged();

    public:
        BuildingHoverInfo *GetHoverDevice() {return buildingHoverInfo;}
    };
}