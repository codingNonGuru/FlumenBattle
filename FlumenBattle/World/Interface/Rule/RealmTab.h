#pragma once

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
}

namespace world::interface::rule
{
    class RuleMenu;
    class EconomyTab;
    class BuildingHoverInfo;

    struct DomainItem : public Element
    {
        Text *nameLabel;

        settlement::Settlement *settlement;

        void HandleConfigure() override;

        void HandleUpdate() override;

        void Setup(settlement::Settlement *);
    };

    class RealmTab : public Element
    {
        LayoutGroup *domainLayout;

        container::Array <DomainItem *> domainItems;

        void HandleConfigure() override;

        void HandleUpdate() override;
    };
}