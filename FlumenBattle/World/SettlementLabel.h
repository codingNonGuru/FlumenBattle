#pragma once

#include "FlumenEngine/Interface/Element.hpp"
#include "FlumenEngine/Interface/Text.hpp"

class Text;
class LayoutGroup;
class ProgressBar;

namespace world::interface
{
    class ResourceCounter;
}

namespace world::settlement
{
    class Settlement;
    enum class ResourceTypes;
    class SettlementLabel;
    class Condition;

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

        struct ConditionWidget : public Text
        {
            friend class HoverExtension;

            using Text::Text;

            ProgressBar *Progress;

            HoverExtension *Parent;

            Condition *Condition {nullptr};

            //bool IsTrackingProduction;

            void HandleConfigure() override;

            void HandleUpdate() override;
        };

        Element *border;

        Text *simulationLabel;

        Text *growthLabel;

        ProgressBar *growthProgress;

        Text *cultureLabel;

        ProgressBar *cultureProgress;

        Text * industryLabel;

        Text * tileLabel;

        Text * factionLabel;

        Text * healthLabel;

        Text * groupLabel;

        Text * buildingLabel;

        const Settlement *settlement;

        LayoutGroup *storageLayout;

        ResourceWidget *foodWidget, *timberWidget, *metalWidget;

        ResourceWidget *foodProduction, *timberProduction, *metalProduction;

        Text *productionLabel;

        ProgressBar *productionProgress;

        container::Array <Text *> pathLabels;

        LayoutGroup *pathLayout;

        container::Array <ConditionWidget *> conditionLabels;

        LayoutGroup *conditionLayout;

        void HandleConfigure() override;

        void HandleUpdate() override;

        void HandleEnable() override;

    public:
        void SetSettlement(const Settlement *settlement) {this->settlement = settlement;}
    };

    class SettlementLabel : public Element
    {
        const Settlement *settlement;

        Text *nameLabel;

        Element *backdrop;

        Text *populationLabel;

        Element *populationBackdrop;

        Element *populationBorder;

        Element *distanceRelatedBackdrop;

        Element *extensionBorder;

        interface::ResourceCounter *garrisonCounter;

        interface::ResourceCounter *wallCounter;

        int garrisonCount;

        int wallsLevel;

        Text *abandonmentLabel;

        void HandleConfigure() override;

        void HandleUpdate() override;

    public:
        void SetSettlement(const Settlement *);

        bool HasSettlement() const {return settlement != nullptr;}

        bool IsSettlementHovered();
    };
}