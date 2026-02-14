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
    class JobHoverInfo;
    class JobItem;
    class ResourceItem;

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

        JobHoverInfo *jobHoverInfo;

        void SetupJobItems();

        void HandleConfigure() override;

        void HandleUpdate() override;

        void HandleEnable() override;

        void HandleSettlementChanged();

    public:
        ResourceHoverInfo *GetHoverDevice() {return resourceHoverInfo;}

        JobHoverInfo *GetJobHoverDevice() {return jobHoverInfo;}
    };
}