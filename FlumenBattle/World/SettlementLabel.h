#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;
class LayoutGroup;
class ProgressBar;

namespace world::settlement
{
    class Settlement;
    enum class ResourceTypes;
    class SettlementLabel;

    class HoverExtension : public Element
    {
        struct ResourceWidget : public Element
        {
            friend class HoverExtension;

            //Element *Icon;

            Text *Label;

            HoverExtension *Parent;

            ResourceTypes Resource;

            bool IsTrackingProduction;

            void HandleConfigure() override;

            void HandleUpdate() override;
        };

        Element *border;

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

        container::Array <Text *> pathLabels;

        LayoutGroup *pathLayout;

        void HandleConfigure() override;

        void HandleUpdate() override;

    public:
        void SetSettlement(Settlement *settlement) {this->settlement = settlement;}
    };

    class SettlementLabel : public Element
    {
        Settlement *settlement;

        Text *nameLabel;

        Element *backdrop;

        Text *populationLabel;

        Element *populationBackdrop;

        Element *populationBorder;

        void HandleConfigure() override;

        void HandleUpdate() override;

    public:
        void SetSettlement(Settlement *);

        bool HasSettlement() const {return settlement != nullptr;}

        bool IsSettlementHovered();
    };
}