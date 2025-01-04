#include "SDL2/SDL.h"

#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"

#include "ImprovementCursor.h"
#include "FlumenBattle/World/WorldController.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Tile/WorldTile.h"
#include "FlumenBattle/World/Group/GroupCore.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/TileImprovement.h"
#include "FlumenBattle/World/Polity/HumanMind.h"

using namespace world::interface;

void ImprovementCursor::HandleConfigure()
{
    backdrop = ElementFactory::BuildElement <Element>
    (
        {Size(110, 35), drawOrder_, {Position2(0.0f, 45.0f), this}, {false}, Opacity(0.7f)},
        2
    );
    backdrop->Enable();

    iconBackdrop = ElementFactory::BuildElement <Element>
    (
        {drawOrder_ + 1, {ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_CENTER, backdrop}, {"WhiteDot", false}, Opacity(0.7f)}
    );
    iconBackdrop->SetTextureScale(0.8f);
    iconBackdrop->Enable();

    icon = ElementFactory::BuildElement <Element>
    (
        {drawOrder_ + 2, {iconBackdrop}, {true}, Opacity(1.0f)}
    );
    icon->Enable();

    label = ElementFactory::BuildText
    (
        {drawOrder_ + 1, {backdrop}},
        {{"Small"}, Color::RED * 0.5f}
    );
    label->Enable();

    WorldController::Get()->OnTileDevelopModeEnabled += {this, &ImprovementCursor::HandleModeEnabled};
}

void ImprovementCursor::HandleModeEnabled() 
{   
    Enable();
}

void ImprovementCursor::HandleEnable() 
{
    SetTextureScale(Scale2(1.5f));

    SDL_ShowCursor(SDL_DISABLE);
}

void ImprovementCursor::HandleDisable() 
{
    SDL_ShowCursor(SDL_ENABLE);
}

void ImprovementCursor::HandleUpdate()
{
    if(WorldController::Get()->IsTileDevelopModeActive() == false)
    {
        Disable();
    }

    static const auto playerGroup = WorldScene::Get()->GetPlayerGroup();
    const auto playerSettlement = playerGroup->GetCurrentSettlement();
    if(playerSettlement == nullptr)
    {
        Disable();
        return;
    }

    auto hoveredTile = WorldController::Get()->GetHoveredTile();
    if(playerSettlement->CanImproveHere(hoveredTile, polity::HumanMind::Get()->GetProposedImprovement()) == false || 
    playerSettlement->IsImprovingTile(hoveredTile, polity::HumanMind::Get()->GetProposedImprovement()) == true)
    {
        SetTexture("X");
    }
    else
    {
        SetTexture("Improve");
    }

    auto type = settlement::TileImprovementFactory::Get()->BuildImprovementType(polity::HumanMind::Get()->GetProposedImprovement());

    label->Setup(type->Name);

    icon->SetTexture(type->TextureName);
}