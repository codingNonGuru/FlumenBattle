#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

namespace world
{
    class Settlement;

    class SettlementLabel : public Element
    {
        Text * nameLabel;

        Text * populationLabel;

        Text * growthLabel;

        Text * foodLabel;

        Text * industryLabel;

        Text * tileLabel;

        Text * healthLabel;

        Text * groupLabel;

        Settlement *settlement;

        void HandleConfigure() override;

        void HandleUpdate() override;

    public:
        void SetSettlement(Settlement *);

        bool HasSettlement() const {return settlement != nullptr;}
    };
}