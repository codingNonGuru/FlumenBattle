#pragma once

#include "FlumenEngine/Interface/Element.hpp"

#include "FlumenBattle/World/Settlement/Types.h"

class Text;

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

        Element *autonomyIcon;

        settlement::Settlement *settlement;

        int population;

        int housing;

        int distance;

        int science;

        int industry;

        RealmTab *parentTab;

        void HandleConfigure() override;

        void HandleUpdate() override;

        void HandleLeftClick() override;

        void Setup(settlement::Settlement *, RealmTab *);

        settlement::Settlement *GetSettlement() {return settlement;}
    };
}