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
    class ResourceCounter;
}

namespace world::interface::rule
{
    class RuleMenu;

    struct DomainItem : public Element
    {
        Text *nameLabel;

        ResourceCounter *populationCounter;

        ResourceCounter *housingCounter;

        ResourceCounter *distanceCounter;

        ResourceCounter *scienceCounter;

        ResourceCounter *industryCounter;

        Element *capitalIcon;

        settlement::Settlement *settlement;

        int population;

        int housing;

        int distance;

        int science;

        int industry;

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