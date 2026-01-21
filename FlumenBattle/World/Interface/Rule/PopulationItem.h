#pragma once

#include "FlumenEngine/Interface/Element.hpp"

namespace world::settlement
{
    class Cohort;
}

namespace world::interface::rule
{
    //class ProductionDecisionInterface;
    class PopulationTab;

    class PopulationItem : public Element
    {
        Text *raceLabel;

        const settlement::Cohort *cohort;

        void HandleConfigure() override;

        void HandleUpdate() override;

    public:
        void Setup(const settlement::Cohort *);
    };
}