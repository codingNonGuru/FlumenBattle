#pragma once

#include "FlumenEngine/Interface/Element.hpp"

#include "FlumenBattle/World/Settlement/Types.h"

class Text;
class LayoutGroup;

namespace world::settlement
{
    class Settlement;
    struct Resource;
}

namespace world::interface
{
    class Counter;
    class ResourceCounter;

    struct ResourceItem : public Element
    {
        friend class RuleMenu;

        Text *nameLabel;

        Text *outputLabel;

        Text *storedLabel;

        Text *inputLabel;

        Element *icon;

        const settlement::Resource *resource;

        void Setup(const settlement::Resource *);

        void HandleConfigure() override;

        void HandleUpdate() override;
    };

    class RuleMenu : public Element
    {
        Text *nameLabel;

        Text *populationLabel;

        Text *rulerLabel;

        Element *border;

        container::Array <ResourceItem *> resourceItems;

        LayoutGroup *itemLayout;

        settlement::Settlement *settlement {nullptr};

        void HandleConfigure() override;

        void HandleUpdate() override;

        void HandleEnable() override;

    public:
        void SetCurrentSettlement(settlement::Settlement *);
    };
}