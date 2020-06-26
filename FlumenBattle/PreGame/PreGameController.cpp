#include "FlumenEngine/Core/InputHandler.hpp"

#include "PreGameController.h"
#include "FlumenBattle/World/WorldState.h"

namespace pregame
{
    void PreGameController::Enable()
    {
        InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_S, {this, &PreGameController::HandleStartGamePressed});
    }

    void PreGameController::Disable()
    {
        InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_S, {this, &PreGameController::HandleStartGamePressed});
    }

    void PreGameController::HandleStartGamePressed()
    {
        world::WorldState::Get()->Enter();
    }
}