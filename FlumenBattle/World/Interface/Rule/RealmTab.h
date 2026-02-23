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
    class DomainHoverInfo;
    class RealmTab;

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

        RealmTab *parentTab;

        void HandleConfigure() override;

        void HandleUpdate() override;

        void Setup(settlement::Settlement *, RealmTab *);

        settlement::Settlement *GetSettlement() {return settlement;}
    };

    class RealmTab : public Element
    {
        LayoutGroup *domainLayout;

        container::Array <DomainItem *> domainItems;

        DomainHoverInfo *hoverInfo;

        void HandleConfigure() override;

        void HandleUpdate() override;

    public:
        DomainHoverInfo *GetHoverInfo() {return hoverInfo;}
    };
}