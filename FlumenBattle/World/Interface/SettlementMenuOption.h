#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

namespace world::interface
{
    class SettlementMenu;
    class ResourceCounter;

    enum class SettlementMenuOptions
    {
        ATTACK,
        BUY_FOOD,
        BUY_MULE,
        SIGN_UP_TO_DELIVER_ITEM,
        FINISH_ITEM_DELIVERY,
        RECRUIT_HEROES
    };

    class SettlementMenuOption : public Element
    {
        Text *label;

        SettlementMenu *menu;

        SettlementMenuOptions option;

        ResourceCounter *priceCounter;

        void HandleConfigure() override;    

        void HandleLeftClick() override;

        void HandleUpdate() override;

    public:
        void Setup(SettlementMenu *, SettlementMenuOptions);
    };
}