#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Core/InputHandler.hpp"

#include "NewGameMenu.h"
#include "FlumenBattle/PreGame/PreGameState.h"

using namespace pregame;

void NewGameMenu::HandleConfigure() 
{
    newWorldLabel = ElementFactory::BuildText(
        {Size(150, 150), drawOrder_ + 1, {Position2(0.0f, -40.0f), this}},
        {{"Large"}, Color::RED * 0.5f, "[N]ew World"}
    );
    newWorldLabel->Enable();

    loadWorldLabel = ElementFactory::BuildText(
        {Size(150, 150), drawOrder_ + 1, {Position2(0.0f, 0.0f), this}},
        {{"Large"}, Color::RED * 0.5f, "[L]oad World"}
    );
    loadWorldLabel->Enable();

    backLabel = ElementFactory::BuildText(
        {Size(150, 150), drawOrder_ + 1, {Position2(0.0f, 40.0f), this}},
        {{"Large"}, Color::RED * 0.5f, "[B]ack"}
    );
    backLabel->Enable();
}

void NewGameMenu::HandleEnable()
{
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_N, {this, &NewGameMenu::OnNewWorldPressed});

    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_B, {this, &NewGameMenu::OnBackPressed});
}

void NewGameMenu::HandleDisable() 
{
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_N);

    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_B);
}

void NewGameMenu::OnNewWorldPressed()
{
    this->Disable();

    PreGameState::Get()->OpenNewWorldMenu();
}

void NewGameMenu::OnBackPressed()
{
    this->Disable();

    PreGameState::Get()->OpenMainMenu();
}