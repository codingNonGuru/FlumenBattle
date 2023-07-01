#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Core/InputHandler.hpp"

#include "GeneratedWorldMenu.h"
#include "FlumenBattle/PreGame/PreGameState.h"

using namespace pregame;

void GeneratedWorldMenu::HandleConfigure() 
{
    startGameLabel = ElementFactory::BuildText(
        {Size(150, 150), drawOrder_ + 1, {Position2(0.0f, -40.0f), this}},
        {{"JSLAncient", "Large"}, Color::RED * 0.5f, "[S]tart Game"}
    );
    startGameLabel->Enable();

    saveWorldLabel = ElementFactory::BuildText(
        {Size(150, 150), drawOrder_ + 1, {Position2(0.0f, 0.0f), this}},
        {{"JSLAncient", "Large"}, Color::RED * 0.5f, "[S]ave World"}
    );
    saveWorldLabel->Enable();

    backLabel = ElementFactory::BuildText(
        {Size(150, 150), drawOrder_ + 1, {Position2(0.0f, 40.0f), this}},
        {{"JSLAncient", "Large"}, Color::RED * 0.5f, "[B]ack"}
    );
    backLabel->Enable();
}

void GeneratedWorldMenu::HandleEnable()
{
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_S, {this, &GeneratedWorldMenu::OnStartGamePressed});

    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_B, {this, &GeneratedWorldMenu::OnBackPressed});
}

void GeneratedWorldMenu::HandleDisable() 
{
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_S, {this, &GeneratedWorldMenu::OnStartGamePressed});

    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_B, {this, &GeneratedWorldMenu::OnBackPressed});
}

void GeneratedWorldMenu::OnStartGamePressed()
{
    PreGameState::Get()->StartGame();

    this->Disable();
}

void GeneratedWorldMenu::OnBackPressed()
{
    this->Disable();
}