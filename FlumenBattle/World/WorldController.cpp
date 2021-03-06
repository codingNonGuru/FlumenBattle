#include "FlumenEngine/Core/InputHandler.hpp"
#include "FlumenEngine/Core/TaskManager.hpp"
#include "FlumenEngine/Sound/SoundManager.h"

#include "FlumenBattle/World/WorldController.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/Battle.h"
#include "FlumenBattle/BattleState.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Group/GroupAction.h"
#include "FlumenBattle/World/Group/HumanController.h"

namespace world
{
    void WorldController::Enable()
    {
        *WorldScene::Get()->OnUpdateStarted += {this, &WorldController::HandleSceneUpdate};

        auto controller = group::HumanController::Get();
        controller->EnableInput();

        InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_SPACE, {this, &WorldController::HandleSpacePressed});
        InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_COMMA, {this, &WorldController::HandleSpeedUpTime});
        InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_PERIOD, {this, &WorldController::HandleSlowDownTime});

        playerBattle = nullptr;
    }

    void WorldController::HandleSceneUpdate()
    {
        auto scene = WorldScene::Get();

        for(auto &battle : scene->GetBattles())
        {
            if(battle.GetFirst() != scene->GetPlayerGroup() && battle.GetSecond() != scene->GetPlayerGroup())
                continue;

            if(!battle.IsOngoing())
                continue;

            playerBattle = &battle;
            break;
        }

        if(playerBattle == nullptr)
            return;

        StartBattle();
    }

    void WorldController::StartBattle()
    {
        auto scene = WorldScene::Get();
        scene->StopTime();

        Disable();

        TaskManager::Add({1.0f, []{
            BattleState::Get()->Enter();
        }});
    }

    void WorldController::HandleSpacePressed()
    {
        WorldScene::Get()->ToggleTime();

        engine::SoundManager::Get()->PlaySound("Click");
    }

    void WorldController::HandleSpeedUpTime()
    {
        WorldScene::Get()->SpeedUpTime();
    }

    void WorldController::HandleSlowDownTime()
    {
        WorldScene::Get()->SlowDownTime();
    }

    void WorldController::Disable()
    {
        *WorldScene::Get()->OnUpdateStarted -= {this, &WorldController::HandleSceneUpdate};

        auto controller = group::HumanController::Get();
        controller->DisableInput();

        InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_SPACE, {this, &WorldController::HandleSpacePressed});
        InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_COMMA, {this, &WorldController::HandleSpeedUpTime});
        InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_PERIOD, {this, &WorldController::HandleSlowDownTime});
    }
}