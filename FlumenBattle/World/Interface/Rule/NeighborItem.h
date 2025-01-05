#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

namespace world::interface
{
    class ResourceCounter;
}

namespace world::polity
{
    struct Neighbor;
}

namespace world::interface::rule
{
    class DiplomacyTab;

    class NeighborItem : public Element
    {
        const polity::Neighbor *neighbor;

        Text *nameLabel;

        ResourceCounter *populationCounter;

        ResourceCounter *settlementCounter;

        ResourceCounter *linkCounter;

        int population;

        int settlementCount;

        int linkCount;

        void HandleConfigure() override;

        void HandleUpdate() override;

    public:
        void Setup(const polity::Neighbor *, DiplomacyTab *);
    };
}