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
    class ProductionDecisionMenu;
    class ResourceCounter;

    class ProductionDecisionItem : public Element
    {
        Text *label;

        settlement::ProductionOptions option;

        ProductionDecisionMenu *parent;

        ResourceCounter *laborCounter;

        void HandleConfigure() override;

        void HandleLeftClick() override;

        void HandleUpdate() override;

    public:
        void Setup(ProductionDecisionMenu *, settlement::ProductionOptions);
    };
}