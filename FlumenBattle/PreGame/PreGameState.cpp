#include "FlumenEngine/Interface/ElementFactory.h"

#include "PreGameState.h"
#include "FlumenBattle/PreGame/PreGameController.h"
#include "FlumenBattle/PreGame/MainMenu.h"

namespace pregame
{
    PreGameState::PreGameState()
    {
        canvas = ElementFactory::BuildCanvas();

        mainMenu = ElementFactory::BuildElement<MainMenu>(
            {Size(480, 300), DrawOrder(3), {Position2(0.0f, 0.0f), canvas}, {"Sprite"}, Opacity(0.85f)}
        );
        mainMenu->Enable();
    }

    void PreGameState::HandleEnter()
    {
        PreGameController::Get()->Enable();

        canvas->Enable();
    }

    void PreGameState::HandleExit() 
    {
        PreGameController::Get()->Disable();

        canvas->Disable();
    }
}