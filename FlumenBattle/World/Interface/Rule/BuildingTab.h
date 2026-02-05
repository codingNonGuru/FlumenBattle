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
    class ProductionDecisionInterface;
    class ProductionQueueItem;

    struct BuildingItem : public Element
    {
        friend class RuleMenu;

        //Text *nameLabel;

        Element *icon;

        Counter *counter;

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

        ProductionDecisionInterface *decisionInterface;

        container::Array <BuildingItem *> buildingItems;

        LayoutGroup *buildingLayout;

        BuildingHoverInfo *buildingHoverInfo;

        container::Array <ProductionQueueItem *> queueItems;

        SimpleList *queueItemList;

        void HandleConfigure() override;

        void HandleUpdate() override;

        void HandleEnable() {}

        void HandleSettlementChanged();

    public:
        BuildingHoverInfo *GetHoverDevice() {return buildingHoverInfo;}
    };
}