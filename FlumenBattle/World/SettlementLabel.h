#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

namespace world::settlement
{
    class Settlement;

    class SettlementLabel : public Element
    {
        Text * nameLabel;

        Text * populationLabel;

        Element * populationBackdrop;

        Element * hoverBackdrop;

        Text * growthLabel;

        Text * foodLabel;

        Text * storageLabel;

        Text * industryLabel;

        Text * tileLabel;

        Text * healthLabel;

        Text * groupLabel;

        Text * metalLabel;

        Element * metalSprite;

        Settlement *settlement;

        void HandleConfigure() override;

        void HandleUpdate() override;

    public:
        void SetSettlement(Settlement *);

        bool HasSettlement() const {return settlement != nullptr;}

        bool IsSettlementHovered();
    };
}