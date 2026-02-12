#pragma once

#include "FlumenEngine/Interface/Element.hpp"

#include "FlumenBattle/World/Settlement/Types.h"

class Text;
class SimpleList;
class ProgressBar;
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
}

namespace world::interface::rule
{
    class RuleMenu;
    class EconomyTab;
    class ResourceHoverInfo;
    class JobItem;

    struct ResourceItem : public Element
    {
        friend class RuleMenu;

        Text *nameLabel;

        Text *outputLabel;

        Text *storedLabel;

        Text *inputLabel;

        Element *icon;

        ProgressBar *storageBar;

        ProgressBar *needBar;

        Text *needLabel;

        Text *workerLabel;

        settlement::Resource *resource;

        const settlement::Settlement *settlement;

        EconomyTab *parentTab;

        void Setup(settlement::Resource *, const settlement::Settlement *, EconomyTab *);

        void HandleConfigure() override;

        void HandleUpdate() override;

        void HandleLeftClick() override;

        void HandleRightClick() override;

        void HandleHover() override;
    };

    class EconomyTab : public Element
    {
        Text *nameLabel;

        Text *populationLabel;

        Text *workerLabel;

        container::Array <ResourceItem *> resourceItems;

        container::Array <JobItem *> jobItems;

        SimpleList *resourceItemLayout;

        LayoutGroup *jobItemLayout;

        ResourceHoverInfo *resourceHoverInfo;

        void SetupJobItems();

        void HandleConfigure() override;

        void HandleUpdate() override;

        void HandleEnable() override;

        void HandleSettlementChanged();

    public:
        ResourceHoverInfo *GetHoverDevice() {return resourceHoverInfo;}
    };
}