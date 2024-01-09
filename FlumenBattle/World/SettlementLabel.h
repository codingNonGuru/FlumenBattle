#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;
class LayoutGroup;
class ProgressBar;

namespace world::settlement
{
    class Settlement;
    enum class ResourceTypes;

    class SettlementLabel : public Element
    {
        struct ResourceWidget : public Element
        {
            friend class SettlementLabel;

            //Element *Icon;

            Text *Label;

            SettlementLabel *Parent;

            ResourceTypes Resource;

            bool IsTrackingProduction;

            void HandleConfigure() override;

            void HandleUpdate() override;
        };

        Text *nameLabel;

        Element *backdrop;

        Text *populationLabel;

        Element *populationBackdrop;

        Element *populationBorder;

        Element *hoverBackdrop;

        Element *hoverBorder;

        Text *growthLabel;

        ProgressBar *growthProgress;

        Text * industryLabel;

        Text * tileLabel;

        Text * healthLabel;

        Text * groupLabel;

        Settlement *settlement;

        LayoutGroup *storageLayout;

        ResourceWidget *foodWidget, *timberWidget, *metalWidget;

        ResourceWidget *foodProduction, *timberProduction, *metalProduction;

        Text *productionLabel;

        ProgressBar *productionProgress;

        void HandleConfigure() override;

        void HandleUpdate() override;

    public:
        void SetSettlement(Settlement *);

        bool HasSettlement() const {return settlement != nullptr;}

        bool IsSettlementHovered();
    };
}