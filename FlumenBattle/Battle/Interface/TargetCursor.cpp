#include "SDL2/SDL.h"

#include "TargetCursor.h"
#include "FlumenBattle/Battle/HumanController.h"
#include "FlumenBattle/Battle/BattleController.h"
#include "FlumenBattle/Battle/Combatant.h"

using namespace battle::interface;

static const auto INSIDE_RANGE_OPACITY = Opacity(1.0f);

static const auto OUTSIDE_RANGE_OPACITY = Opacity(0.4f);

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
        SetOpacity(OUTSIDE_RANGE_OPACITY);
        return;
    }

    auto yourSelectedCombatant = BattleController::Get()->GetSelectedCombatant();

    bool isWithingRange = yourSelectedCombatant->IsWithinActionRange(hoveredTile);
    if(isWithingRange)
    {
        SetOpacity(INSIDE_RANGE_OPACITY);
    }
    else
    {
        SetOpacity(OUTSIDE_RANGE_OPACITY);
    }
}