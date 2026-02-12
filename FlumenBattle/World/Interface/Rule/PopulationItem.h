#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class ProgressBar;

namespace world::settlement
{
    class Cohort;
    class Settlement;
}

namespace world::interface::rule
{
    //class ProductionDecisionInterface;
    class PopulationTab;

    class PopulationItem : public Element
    {
        Text *raceLabel;

        Element *raceIcon;

        ProgressBar *healthBar;

        Element *jobSprite;

        const settlement::Cohort *cohort;

        const settlement::Settlement *settlement;

        void HandleConfigure() override;

        void HandleUpdate() override;

        void HandleLeftClick() override;

    public:
        void Setup(const settlement::Cohort *, const settlement::Settlement *);
    };
}