#include "PreGameState.h"
#include "FlumenBattle/PreGame/PreGameController.h"

namespace pregame
{
    void PreGameState::HandleEnter()
    {
        PreGameController::Get()->Enable();
    }

    void PreGameState::HandleExit() 
    {
        PreGameController::Get()->Disable();
    }
}