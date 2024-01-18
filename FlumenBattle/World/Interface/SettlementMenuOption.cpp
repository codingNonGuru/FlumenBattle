#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"

#include "SettlementMenuOption.h"
#include "FlumenBattle/World/Interface/SettlementMenu.h"

using namespace world::interface;

static constexpr auto DEFAULT_FONT_TYPE = "JSLAncient";

static constexpr auto OPTION_FONT_SIZE = "Small";

static constexpr auto BASE_OPTION_OPACITY = 0.5f;

static constexpr auto HOVERED_OPTION_OPACITY = 1.0f;

void SettlementMenuOption::HandleConfigure() 
{
    label = ElementFactory::BuildText
    (
        {
            Size(), 
            drawOrder_ + 1, 
            {Position2(10.0f, 3.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}
        },
        {
            {DEFAULT_FONT_TYPE, OPTION_FONT_SIZE}, 
            Color::RED * 0.5f, 
            "Buy food"
        }
    );
    label->Enable();
}

void SettlementMenuOption::HandleLeftClick()
{
    menu->ProcessOptionInput();
}

void SettlementMenuOption::HandleUpdate()
{
    if(isHovered_)
    {
        SetOpacity(HOVERED_OPTION_OPACITY);
    }
    else
    {
        SetOpacity(BASE_OPTION_OPACITY);
    }
}