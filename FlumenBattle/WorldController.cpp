#include "FlumenEngine/Core/InputHandler.hpp"
#include "FlumenEngine/Core/TaskManager.hpp"

#include "FlumenBattle/WorldController.h"
#include "FlumenBattle/WorldScene.h"
#include "FlumenBattle/BattleState.h"
#include "FlumenBattle/Group.h"

void WorldController::Enable()
{
    EnableInput();
}

void WorldController::EnableInput()
{
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_S, this, &WorldController::HandleSPressed);

    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_R, this, &WorldController::HandleRPressed);
}

void WorldController::DisableInput()
{
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_S, this, &WorldController::HandleSPressed);

    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_R, this, &WorldController::HandleRPressed);
}

void WorldController::HandleSPressed()
{
    TaskManager::Add()->Initialize(this, &WorldController::StartBattle, 1.0f);

    DisableInput();
}

void WorldController::HandleRPressed()
{
    TaskManager::Add()->Initialize(this, &WorldController::Rest, 1.0f);

    DisableInput();
}

void WorldController::StartBattle()
{
    WorldScene::Get()->StartBattle();

    BattleState::Get()->Enter();
}

void WorldController::Rest()
{
    WorldScene::Get()->Rest();

    EnableInput();
}

void WorldController::Disable()
{
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_S, this, &WorldController::HandleSPressed);

    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_R, this, &WorldController::HandleRPressed);
}