#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

namespace world::interface
{
    class SettlementMenu;
    class ResourceCounter;

    class SettlementMenuOption : public Element
    {
        Text *label;

        SettlementMenu *menu;

        ResourceCounter *priceCounter;

        void HandleConfigure() override;    

        void HandleLeftClick() override;

        void HandleUpdate() override;

    public:
        void Setup(SettlementMenu *newMenu) {menu = newMenu;}
    };
}