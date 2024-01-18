#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;
class LayoutGroup;

namespace world::settlement
{
    class Settlement;
}

namespace world::interface
{
    class SettlementMenu : public Element
    {
        class Option : public Element
        {
            Text *label;

            SettlementMenu *menu;

            void HandleConfigure() override;    

            void HandleLeftClick() override;

            void HandleUpdate() override;

        public:
            void Setup(SettlementMenu *newMenu) {menu = newMenu;}
        };

        Element *border;

        Text *nameLabel;

        LayoutGroup *optionLayout;

        settlement::Settlement *currentSettlement {nullptr};

        void HandleConfigure() override;

        void HandleUpdate() override;

    public:
        void Setup(settlement::Settlement *);

        void ProcessOptionInput();
    };
}