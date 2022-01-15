#include "IntroState.h"
#include "FlumenBattle/PreGame/PreGameState.h"
//#include "FlumenBattle/PreGame/PreGameController.h"

namespace intro
{
    void IntroState::HandleEnter()
    {
        pregame::PreGameState::Get()->Enter();
    }

    void IntroState::HandleExit() {}
}