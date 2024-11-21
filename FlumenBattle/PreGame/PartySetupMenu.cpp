#include "FlumenEngine/Core/InputHandler.hpp"
#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"

#include "PartySetupMenu.h"
#include "FlumenBattle/PreGame/PreGameState.h"

using namespace pregame;

static const auto TEXT_COLOR = Color::RED * 0.5f;

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto BORDER_INSET = Size(4, 4);

void PartySetupMenu::HandleConfigure()
{
    startGameLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(0.0f, -60.0f), this}},
        {{"Large"}, TEXT_COLOR, "[S]tart Game"}
    );
    startGameLabel->Enable();

    savePresetLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(0.0f, -20.0f), this}},
        {{"Large"}, TEXT_COLOR, "Sa[v]e Preset"}
    );
    savePresetLabel->Enable();

    loadPresetLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(0.0f, 20.0f), this}},
        {{"Large"}, TEXT_COLOR, "[L]oad Preset"}
    );
    loadPresetLabel->Enable();

    backLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(0.0f, 60.0f), this}},
        {{"Large"}, TEXT_COLOR, "[B]ack"}
    );
    backLabel->Enable();

    border = ElementFactory::BuildElement <Element>
    (
        {
            size_ - BORDER_INSET, 
            drawOrder_ + 1, 
            {this}, 
            {"panel-border-031", true}
        }
    );
    border->SetSpriteColor(BORDER_COLOR);
    border->Enable();
}

void PartySetupMenu::HandleEnable()
{
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_S, {this, &PartySetupMenu::OnStartGamePressed});

    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_B, {this, &PartySetupMenu::OnBackPressed});
}

void PartySetupMenu::HandleDisable()
{
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_S);

    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_B);
}

void PartySetupMenu::OnStartGamePressed()
{
    Disable();

    PreGameState::Get()->StartGame();
}

void PartySetupMenu::OnBackPressed()
{
    Disable();

    PreGameState::Get()->OpenGeneratedWorldMenu();
}