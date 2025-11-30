#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Core/InputHandler.hpp"

#include "MainMenu.h"
#include "FlumenBattle/PreGame/PreGameState.h"

using namespace pregame;

static const auto TEXT_COLOR = Color::RED * 0.5f;

static const auto HIGHLIGHT_COLOR = Color::RED * 0.9f;

void MainMenu::HandleConfigure() 
{
    newGameLabel = ElementFactory::BuildRichText(
        {Size(150, 150), drawOrder_ + 1, {Position2(0.0f, -20.0f), this}},
        {{"Large"}, TEXT_COLOR, HIGHLIGHT_COLOR, "<2>N<1>ew Game"}
    );
    newGameLabel->Enable();

    quitGameLabel = ElementFactory::BuildRichText(
        {Size(150, 150), drawOrder_ + 1, {Position2(0.0f, 20.0f), this}},
        {{"Large"}, TEXT_COLOR, HIGHLIGHT_COLOR, "<2>E<1>xit"}
    );
    quitGameLabel->Enable();
}

void MainMenu::HandleEnable()
{
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_N, {this, &MainMenu::HandleNewGamePressed});

    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_X, {this, &MainMenu::OnExitPressed});
}

void MainMenu::HandleDisable() 
{
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_N);

    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_X);
}

void MainMenu::HandleNewGamePressed()
{
    this->Disable();

    PreGameState::Get()->OpenNewGameMenu();
}

void MainMenu::OnExitPressed()
{
    this->Disable();
}