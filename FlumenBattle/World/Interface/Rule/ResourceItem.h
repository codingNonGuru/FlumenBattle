#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;
class ProgressBar;

namespace world::settlement
{
    class Settlement;
    struct Resource;
    struct Building;
}

namespace world::interface
{
    class ResourceCounter;
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

        ResourceCounter *buildingCounter;

        settlement::Resource *resource;

        const settlement::Settlement *settlement;

        EconomyTab *parentTab;

        int buildingCount;

        void Setup(settlement::Resource *, const settlement::Settlement *, EconomyTab *);

        void HandleConfigure() override;

        void HandleUpdate() override;

        void HandleLeftClick() override;

        void HandleRightClick() override;

        void HandleHover() override;
    };
}