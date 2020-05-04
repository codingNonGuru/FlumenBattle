#include "FlumenEngine/Core/InputHandler.hpp"
#include "FlumenEngine/Core/TaskManager.hpp"

#include "FlumenBattle/WorldController.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/BattleState.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Group/GroupAction.h"
#include "FlumenBattle/World/Group/HumanController.h"

void WorldController::Enable()
{
    auto controller = world::group::HumanController::Get();
    controller->EnableInput();
    //controller->OnActionPerformed->Add(this, &WorldController::HandleActionPerformed);

    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_SPACE, this, &WorldController::HandleSpacePressed);
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_COMMA, this, &WorldController::HandleSpeedUpTime);
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_PERIOD, this, &WorldController::HandleSlowDownTime);
}

void WorldController::HandleActionPerformed()
{
    
}

void WorldController::StartBattle()
{
    //world::WorldScene::Get()->StartBattle();

    //BattleState::Get()->Enter();
}

void WorldController::HandleSpacePressed()
{
    world::WorldScene::Get()->ToggleTime();
}

void WorldController::HandleSpeedUpTime()
{
    world::WorldScene::Get()->SpeedUpTime();
}

void WorldController::HandleSlowDownTime()
{
    world::WorldScene::Get()->SlowDownTime();
}

void WorldController::Disable()
{
    auto controller = world::group::HumanController::Get();
    controller->DisableInput();
    //controller->OnActionPerformed->Remove(this, &WorldController::HandleActionPerformed);

    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_SPACE, this, &WorldController::HandleSpacePressed);
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_COMMA, this, &WorldController::HandleSpeedUpTime);
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_PERIOD, this, &WorldController::HandleSlowDownTime);
}