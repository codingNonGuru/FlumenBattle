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
    class ProductionDecisionItem;
    class ResourceCounter;

    class ProductionDecisionMenu : public Element
    {
        Element *border;

        Text *nameLabel;

        Text *productionLabel;

        ProgressBar *productionProgress;

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