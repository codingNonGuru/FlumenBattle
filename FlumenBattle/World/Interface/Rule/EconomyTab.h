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
    class EconomyTab;
    class BuildingHoverInfo;

    struct ResourceItem : public Element
    {
        friend class RuleMenu;

        Text *nameLabel;

        Text *outputLabel;

        Text *storedLabel;

        Text *inputLabel;

        Element *icon;

        ProgressBar *needBar;

        Text *needLabel;

        const settlement::Resource *resource;

        const settlement::Settlement *settlement;

        void Setup(const settlement::Resource *, const settlement::Settlement *);

        void HandleConfigure() override;

        void HandleUpdate() override;
    };

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

        EconomyTab *parentTab;

        void Setup(settlement::Building *, EconomyTab *);

        void HandleConfigure() override;

        void HandleUpdate() override;

        void HandleHover() override;

        void HandleLeftClick() override;

        void HandleRightClick() override;

    public:
        const settlement::Building *GetBuilding() const {return building;}
    };

    class EconomyTab : public Element
    {
        Text *nameLabel;

        Text *populationLabel;

        Text *workerLabel;

        container::Array <ResourceItem *> resourceItems;

        container::Array <BuildingItem *> buildingItems;

        LayoutGroup *itemLayout;

        LayoutGroup *buildingLayout;

        BuildingHoverInfo *buildingHoverInfo;

        void HandleConfigure() override;

        void HandleUpdate() override;

        void HandleEnable() override;

        void HandleSettlementChanged();

    public:
        BuildingHoverInfo *GetHoverDevice() {return buildingHoverInfo;}
    };
}