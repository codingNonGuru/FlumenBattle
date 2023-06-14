#include "FlumenCore/Time.hpp"

#include "FlumenEngine/Core/InputHandler.hpp"
#include "FlumenEngine/Core/TaskManager.hpp"
#include "FlumenEngine/Sound/SoundManager.h"
#include "FlumenEngine/Render/Camera.hpp"
#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/MeshManager.hpp"
#include "FlumenEngine/Utility/PickHandler.h"

#include "FlumenBattle/World/WorldController.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/WorldMap.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/Battle.h"
#include "FlumenBattle/BattleState.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Group/GroupAction.h"
#include "FlumenBattle/World/Group/HumanController.h"
#include "FlumenBattle/World/Settlement/Settlement.h"

static const Float CAMERA_PAN_SPEED = 800.0f;

static const Float CAMERA_ZOOM_SPEED = 3.0f;

static Camera *camera = nullptr;

namespace world
{
    void WorldController::Enable()
    {
        playerBattle = nullptr;

        *WorldScene::Get()->OnUpdateStarted += {this, &WorldController::HandleSceneUpdate};

        auto controller = group::HumanController::Get();
        controller->EnableInput();

        InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_SPACE, {this, &WorldController::HandleSpacePressed});
        InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_COMMA, {this, &WorldController::HandleSpeedUpTime});
        InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_PERIOD, {this, &WorldController::HandleSlowDownTime});

        InputHandler::RegisterContinualEvent(SDL_Scancode::SDL_SCANCODE_UP, {this, &WorldController::HandlePanUp});
        InputHandler::RegisterContinualEvent(SDL_Scancode::SDL_SCANCODE_DOWN, {this, &WorldController::HandlePanDown});
        InputHandler::RegisterContinualEvent(SDL_Scancode::SDL_SCANCODE_RIGHT, {this, &WorldController::HandlePanRight});
        InputHandler::RegisterContinualEvent(SDL_Scancode::SDL_SCANCODE_LEFT, {this, &WorldController::HandlePanLeft});

        camera = RenderManager::GetCamera(Cameras::WORLD);
    }

    void WorldController::CheckTileSelection()
    {
        auto mesh = MeshManager::GetMesh("Hex"); 

        auto map = WorldScene::Get()->GetWorldMap();
        for(auto tile = map->tiles.GetStart(); tile != map->tiles.GetEnd(); ++tile) 
        {
            bool isMouseOverTile = PickHandler::CheckCollision(camera, mesh, Position3(tile->Position, 0.0f), 33.0f); 
            if(isMouseOverTile)
            {
                hoveredTile = tile;
                return;
            }
        }

        hoveredTile = nullptr;
    }

    void WorldController::HandleSceneUpdate()
    {
        CheckTileSelection();

        auto zoomSpeed = CAMERA_ZOOM_SPEED * Time::GetDelta();

        if(InputHandler::GetMouse().ScrollUp_)
        {
            camera->Zoom(1.0f - zoomSpeed);
        }
        else if(InputHandler::GetMouse().ScrollDown_)
        {
            camera->Zoom(1.0f + zoomSpeed);
        }

        auto scene = WorldScene::Get();

        for(auto &battle : scene->GetBattles())
        {
            std::cout<<"aloha\n";
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
        std::cout<<"start battle\n";
        auto scene = WorldScene::Get();
        scene->StopTime();

        Disable();

        TaskManager::Add({1.0f, []{
            BattleState::Get()->Enter();
        }});
    }

    void WorldController::HandlePanUp()
    {
        auto panSpeed = (CAMERA_PAN_SPEED * camera->GetZoomFactor()) * Time::GetDelta();        
        camera->Translate(Direction3(0.0f, -panSpeed, 0.0f));
    }

    void WorldController::HandlePanDown()
    {
        auto panSpeed = (CAMERA_PAN_SPEED * camera->GetZoomFactor()) * Time::GetDelta();        
        camera->Translate(Direction3(0.0f, panSpeed, 0.0f));
    }

    void WorldController::HandlePanLeft() 
    {
        auto panSpeed = (CAMERA_PAN_SPEED * camera->GetZoomFactor()) * Time::GetDelta();        
        camera->Translate(Direction3(-panSpeed, 0.0f, 0.0f));
    }

    void WorldController::HandlePanRight() 
    {
        auto panSpeed = (CAMERA_PAN_SPEED * camera->GetZoomFactor()) * Time::GetDelta();        
        camera->Translate(Direction3(panSpeed, 0.0f, 0.0f));
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

        InputHandler::UnregisterContinualEvent(SDL_Scancode::SDL_SCANCODE_UP, {this, &WorldController::HandlePanUp});
        InputHandler::UnregisterContinualEvent(SDL_Scancode::SDL_SCANCODE_DOWN, {this, &WorldController::HandlePanDown});
        InputHandler::UnregisterContinualEvent(SDL_Scancode::SDL_SCANCODE_RIGHT, {this, &WorldController::HandlePanRight});
        InputHandler::UnregisterContinualEvent(SDL_Scancode::SDL_SCANCODE_LEFT, {this, &WorldController::HandlePanLeft});
    }
}