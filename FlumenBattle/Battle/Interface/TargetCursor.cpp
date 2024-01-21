#include "SDL2/SDL.h"

#include "TargetCursor.h"
#include "FlumenBattle/Battle/HumanController.h"
#include "FlumenBattle/Battle/BattleController.h"
#include "FlumenBattle/Battle/Combatant.h"

using namespace battle::interface;

static const auto INSIDE_RANGE_TEXTURE = "SwordCursor";

static const auto OUTSIDE_RANGE_TEXTURE = "X";

void TargetCursor::HandleEnable() 
{
    SDL_ShowCursor(SDL_DISABLE);
}

void TargetCursor::HandleDisable() 
{
    SDL_ShowCursor(SDL_ENABLE);
}

void TargetCursor::HandleUpdate()
{
    auto hoveredTile = HumanController::Get()->GetHoveredTile();
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
    }
}