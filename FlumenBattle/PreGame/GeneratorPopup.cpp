#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Core/InputHandler.hpp"

#include "GeneratorPopup.h"
#include "FlumenBattle/PreGame/PreGameState.h"
#include "FlumenBattle/World/WorldGenerator.h"

using namespace pregame;

void GeneratorPopup::HandleConfigure() 
{
    loadingLabel = ElementFactory::BuildText(
        {Size(150, 150), drawOrder_ + 1, {Position2(0.0f, -20.0f), this}},
        {{"JSLAncient", "Large"}, Color::RED * 0.5f, "Loading..."}
    );
    loadingLabel->Enable();
}

static int x = 0;

void GeneratorPopup::HandleUpdate() 
{
    x++;
    if(x > 40)
    {
        x = 0;
    }

    loadingLabel->Setup(x < 10 ? "Loading" : (x < 20 ? "Loading." : (x < 30 ? "Loading.." : "Loading...")));

    if(world::WorldGenerator::Get()->HasFinishedGenerating())
    {
        PreGameState::Get()->FinishWorldGeneration();
    }
}

void GeneratorPopup::HandleEnable()
{
    /*InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_N, {this, &MainMenu::HandleNewGamePressed});

    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_X, {this, &MainMenu::OnExitPressed});*/
}

void GeneratorPopup::HandleDisable() 
{
    /*InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_N, {this, &MainMenu::HandleNewGamePressed});

    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_X, {this, &MainMenu::OnExitPressed});*/
}

/*void MainMenu::HandleNewGamePressed()
{
    this->Disable();

    PreGameState::Get()->OpenNewGameMenu();
}

void MainMenu::OnExitPressed()
{
    this->Disable();
}*/