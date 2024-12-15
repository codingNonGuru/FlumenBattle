#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;
class SimpleList;
class ProgressBar;
class LayoutGroup;

namespace world::settlement
{
    class Settlement;
    enum class ProductionOptions;
}

namespace world::interface
{
    class ResourceCounter;
}

namespace world::interface::rule
{
    class ProductionDecisionItem;

    class ProductionDecisionInterface : public Element
    {
        Text *productionLabel;

        ProgressBar *productionProgress;

        Text *timeLeftLabel;

        ResourceCounter *laborCounter;

        SimpleList *optionLayout;

        container::Array <ProductionDecisionItem *> optionItems;

        settlement::Settlement *currentSettlement;

        int industrialCapacity;

        void HandleConfigure() override;

        void HandleUpdate() override;

    public:
        void Setup(settlement::Settlement *);

        void ProcessInput(settlement::ProductionOptions);
    };
}