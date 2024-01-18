#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

namespace world::interface
{
    class SettlementMenu;

    class SettlementMenuOption : public Element
    {
        Text *label;

        SettlementMenu *menu;

        void HandleConfigure() override;    

        void HandleLeftClick() override;

        void HandleUpdate() override;

    public:
        void Setup(SettlementMenu *newMenu) {menu = newMenu;}
    };
}