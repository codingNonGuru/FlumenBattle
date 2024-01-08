#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

namespace world::settlement
{
    class Settlement;

    class SettlementLabel : public Element
    {
        Text *nameLabel;

        Element *backdrop;

        Text *populationLabel;

        Element *populationBackdrop;

        Element *populationBorder;

        Element *hoverBackdrop;

        Element *hoverBorder;

        Text * growthLabel;

        Text *foodLabel;

        Text *storageLabel;

        Element *foodIcon;

        Text * industryLabel;

        Text * tileLabel;

        Text * healthLabel;

        Text * groupLabel;

        Text * metalLabel;

        Element * metalIcon;

        Settlement *settlement;

        void HandleConfigure() override;

        void HandleUpdate() override;

    public:
        void SetSettlement(Settlement *);

        bool HasSettlement() const {return settlement != nullptr;}

        bool IsSettlementHovered();
    };
}