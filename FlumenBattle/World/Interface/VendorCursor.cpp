#include "SDL2/SDL.h"

#include "VendorCursor.h"
#include "FlumenBattle/World/Group/HumanMind.h"

using namespace world::interface;

static const auto TEXTURE = "Coin";

void VendorCursor::HandleEnable() 
{
    SDL_ShowCursor(SDL_DISABLE);
}

void VendorCursor::HandleDisable() 
{
    SDL_ShowCursor(SDL_ENABLE);
}

void VendorCursor::HandleUpdate()
{
    if(world::group::HumanMind::Get()->IsSellModeActive() == false)
    {
        Disable();
    }
    /*auto hoveredTile = HumanController::Get()->GetHoveredTile();
    if(hoveredTile == nullptr)
    {
        SetTexture(OUTSIDE_RANGE_TEXTURE);
        return;
    }

    auto yourSelectedCombatant = BattleController::Get()->GetSelectedCombatant();

    bool isWithingRange = yourSelectedCombatant->IsWithinActionRange(hoveredTile);
    if(isWithingRange)
    {
        SetTexture(INSIDE_RANGE_TEXTURE);
    }
    else
    {
        SetTexture(OUTSIDE_RANGE_TEXTURE);
    }*/
}