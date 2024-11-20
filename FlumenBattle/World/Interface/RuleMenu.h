#pragma once

#include "FlumenEngine/Interface/Element.hpp"

#include "FlumenBattle/World/Settlement/Types.h"

class Text;
class LayoutGroup;

namespace world::settlement
{
    class Settlement;
}

namespace world::interface
{
    class Counter;
    class ResourceCounter;

    struct ResourceItem : public Element
    {
        friend class RuleMenu;

        Text *nameLabel;

        Element *border;

        settlement::ResourceTypes resourceType;

        void Setup(settlement::ResourceTypes);

        void HandleConfigure() override;
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