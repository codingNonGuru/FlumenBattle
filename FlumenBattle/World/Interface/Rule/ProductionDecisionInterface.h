#pragma once

#include "FlumenEngine/Interface/Element.hpp"

#include "FlumenBattle/World/Settlement/Types.h"

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
    struct ProductionInterfaceData : public AdditionalElementData
    {
        settlement::ProductionClasses Type;

        ProductionInterfaceData(settlement::ProductionClasses type) : Type(type) {}
    };

    class ProductionDecisionItem;
    
    class ProductionDecisionInterface : public Element
    {
        settlement::ProductionClasses productionType;

        Text *productionLabel;

        ProgressBar *productionProgress;

        Text *timeLeftLabel;

        ResourceCounter *laborCounter;

        SimpleList *optionLayout;

        container::Array <ProductionDecisionItem *> optionItems;

        settlement::Settlement *currentSettlement;

        int industrialCapacity;

        void HandleConfigure(AdditionalElementData *) override;

        void HandleUpdate() override;

    public:
        void Setup(settlement::Settlement *);

        void ProcessInput(settlement::ProductionOptions);
    };
}