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
#include "FlumenBattle/World/Group/Encounter.h"
#include "FlumenBattle/Battle/BattleState.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Group/GroupAction.h"
#include "FlumenBattle/World/Group/HumanMind.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/Utility/Pathfinder.h"

static const Float CAMERA_PAN_SPEED = 800.0f;

static const Float CAMERA_ZOOM_SPEED = 2.0f;

static const Float TRANSITION_TO_BATTLE_DELAY = 0.5f;

static const Float CAMERA_GRAB_SENSITIVITY = 1.7f;

static Camera *camera = nullptr;

const int PLANNED_PATH_MAXIMUM_SIZE = 12;

static const auto TRAVEL_MODE_INPUT_KEY = SDL_Scancode::SDL_SCANCODE_T;

static const auto SCREEN_GRAB_INPUT_KEY = SDL_Scancode::SDL_SCANCODE_LALT;

namespace world
{
    WorldController::WorldController() {}

    void WorldController::Initialize()
    {
        WorldScene::Get()->OnPlayerEncounterInitiated += {this, &WorldController::HandlePlayerEncounterInitiated};

        WorldScene::Get()->OnPlayerEncounterFinished += {this, &WorldController::DisableEncounterMode};

        WorldScene::Get()->OnPlayerBattleStarted += {this, &WorldController::HandleBattleStarted};

        group::HumanMind::Get();
    }

    void WorldController::Enable()
    {
        if(GetPlayerBattle() != nullptr)
        {
        }
        else
        {
            EnableHardInput();
        }

        InputHandler::RegisterContinualEvent(SDL_Scancode::SDL_SCANCODE_UP, {this, &WorldController::HandlePanUp});
        InputHandler::RegisterContinualEvent(SDL_Scancode::SDL_SCANCODE_DOWN, {this, &WorldController::HandlePanDown});
        InputHandler::RegisterContinualEvent(SDL_Scancode::SDL_SCANCODE_RIGHT, {this, &WorldController::HandlePanRight});
        InputHandler::RegisterContinualEvent(SDL_Scancode::SDL_SCANCODE_LEFT, {this, &WorldController::HandlePanLeft});
        InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_M, {this, &WorldController::HandleResourceDisplayPressed});
        InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_1, {this, &WorldController::HandleCharacterSelected});
        InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_2, {this, &WorldController::HandleCharacterSelected});
        InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_3, {this, &WorldController::HandleCharacterSelected});
        InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_4, {this, &WorldController::HandleCharacterSelected});
        InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_5, {this, &WorldController::HandleCharacterSelected});
        InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_6, {this, &WorldController::HandleCharacterSelected});
        InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_I, {this, &WorldController::HandleInventoryPressed});

        InputHandler::RegisterContinualEvent(SCREEN_GRAB_INPUT_KEY, {this, &WorldController::HandleGrabPressed}, {this, &WorldController::HandleGrabReleased});

        //InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_C, {this, &WorldController::HandleColonizationSwitch});

        camera = RenderManager::GetCamera(Cameras::WORLD);
        camera->EnableDynamicZooming();
    }

    void WorldController::HandleColonizationSwitch()
    {
        canColonize = canColonize ? false : true;
    }

    void WorldController::HandlePlayerEncounterInitiated()
    {
        isInEncounterMode = true;

        DisableHardInput();
    }

    void WorldController::DisableEncounterMode()
    {
        isInEncounterMode = false;

        EnableHardInput();
    }

    void WorldController::DisableHardInput()
    {
        auto controller = group::HumanMind::Get();
        controller->DisableInput();

        InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_SPACE);
        InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_COMMA);
        InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_PERIOD);
    }

    void WorldController::EnableHardInput()
    {
        auto controller = group::HumanMind::Get();
        controller->EnableInput();

        InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_SPACE, {this, &WorldController::HandleSpacePressed});
        InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_COMMA, {this, &WorldController::HandleSpeedUpTime});
        InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_PERIOD, {this, &WorldController::HandleSlowDownTime});
    }

    void WorldController::CheckTileSelection()
    {
        auto map = WorldScene::Get()->GetWorldMap();

        auto camera = RenderManager::GetCamera(Cameras::WORLD);

        auto mousePosition = InputHandler::GetMousePosition();
        auto worldPosition = camera->GetWorldPosition(mousePosition);

        auto tile = map->GetTile(worldPosition);
        if(tile == nullptr)
        {
            hoveredTile = nullptr;
        }
        else
        {
            auto nearbyTiles = tile->GetNearbyTiles(1);

            auto mesh = MeshManager::GetMesh("Hex"); 
            for(auto &nearbyTile : nearbyTiles)
            {
                bool isMouseOverTile = PickHandler::CheckCollision(camera, mesh, Position3(nearbyTile->Position, 0.0f), WorldMap::WORLD_TILE_SIZE); 
                if(isMouseOverTile)
                {
                    hoveredTile = nearbyTile;
                    return;
                }   
            }
        }
    }

    static utility::PathData <WorldTile> plannedPath;

    void WorldController::Update()
    {
        CheckTileSelection();

        auto zoomSpeed = CAMERA_ZOOM_SPEED * Time::GetDelta();

        if(InputHandler::GetMouse().ScrollUp_)
        {
            camera->ZoomDynamically(zoomSpeed);
        }
        else if(InputHandler::GetMouse().ScrollDown_)
        {
            camera->ZoomDynamically(-zoomSpeed);
        }

        if(InputHandler::IsPressed(TRAVEL_MODE_INPUT_KEY) == true)
        {
            isTravelPlanActive = true;
        }
        else
        {
            isTravelPlanActive = false;
        }

        if(isGrabbingScreen == true)
        {
            camera->Translate({grabPositionDelta.x, grabPositionDelta.y, 0.0f});

            auto mouseDelta = InputHandler::GetMousePosition() - grabStartPosition;
            
            mouseDelta *= CAMERA_GRAB_SENSITIVITY;

            mouseDelta *= camera->GetZoomFactor();

            camera->Translate({-mouseDelta.x, -mouseDelta.y, 0.0f});

            grabPositionDelta = mouseDelta;
        }

        if(hoveredTile == nullptr)
            return;

        auto playerGroup = WorldScene::Get()->GetPlayerGroup();
        auto playerLocation = playerGroup->GetTile();

        if(hoveredTile == playerLocation || playerLocation->GetDistanceTo(*hoveredTile) >= PLANNED_PATH_MAXIMUM_SIZE || isTravelPlanActive == false)
            return;

        plannedPath = utility::Pathfinder <WorldTile>::Get()->FindPathAsGeneric(hoveredTile, playerLocation, PLANNED_PATH_MAXIMUM_SIZE - 4);
    }

    void WorldController::HandleBattleStarted()
    {
        std::cout<<"start battle\n";
        auto scene = WorldScene::Get();
        scene->StopTime();

        Disable();

        TaskManager::Add({TRANSITION_TO_BATTLE_DELAY, []{
            battle::BattleState::Get()->Enter();
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

    void WorldController::HandleGrabPressed()
    {
        isGrabbingScreen = true;
        grabStartPosition = InputHandler::GetMousePosition();

        grabPositionDelta = Position2();
    }

    void WorldController::HandleGrabReleased()
    {
        isGrabbingScreen = false;
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

    void WorldController::HandleResourceDisplayPressed()
    {
        if(isResourceDisplayActive)
        {
            isResourceDisplayActive = false;
        }
        else
        {
            isResourceDisplayActive = true;
        }
    }

    void WorldController::HandleCharacterSelected()
    {
        int index = 0;
        if(InputHandler::IsPressed(SDL_SCANCODE_1))
        {
            index = 0;
        }
        else if(InputHandler::IsPressed(SDL_SCANCODE_2))
        {
            index = 1;
        }
        else if(InputHandler::IsPressed(SDL_SCANCODE_3))
        {
            index = 2;
        }
        else if(InputHandler::IsPressed(SDL_SCANCODE_4))
        {
            index = 3;
        }
        else if(InputHandler::IsPressed(SDL_SCANCODE_5))
        {
            index = 4;
        }
        else if(InputHandler::IsPressed(SDL_SCANCODE_6))
        {
            index = 5;
        }

        if(characterSelection.IsSelected == true)
        {
            if(characterSelection.Index == index)
            {
                characterSelection.IsSelected = false;
            }
            else
            {
                characterSelection.Index = index;
            }
        }
        else
        {
            characterSelection.IsSelected = true;
            characterSelection.Index = index;
        }

        onCharacterSelected.Invoke();
    }

    void WorldController::HandleInventoryPressed()
    {
        characterSelection.IsSelected = true;

        onInventoryPressed.Invoke();
    }

    void WorldController::Disable()
    {
        DisableHardInput();

        InputHandler::UnregisterContinualEvent(SDL_Scancode::SDL_SCANCODE_UP);
        InputHandler::UnregisterContinualEvent(SDL_Scancode::SDL_SCANCODE_DOWN);
        InputHandler::UnregisterContinualEvent(SDL_Scancode::SDL_SCANCODE_RIGHT);
        InputHandler::UnregisterContinualEvent(SDL_Scancode::SDL_SCANCODE_LEFT);
        InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_M);
        InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_1);
        InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_2);
        InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_3);
        InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_4);
        InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_5);
        InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_6);
        InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_I);

        InputHandler::UnregisterContinualEvent(SDL_Scancode::SDL_SCANCODE_LALT);
    }

    bool WorldController::CanBuyFood()
    {
        auto playerGroup = WorldScene::Get()->GetPlayerGroup();

        auto playerSettlement = playerGroup->GetCurrentSettlement();
        if(playerSettlement == nullptr)
            return false;

        auto playerMoney = playerGroup->GetMoney();

        auto foodResource = playerSettlement->GetResource(settlement::ResourceTypes::FOOD);
        if(playerMoney < foodResource->Type->Value)
            return false;

        if(foodResource->Storage == 0)
            return false;

        return true;
    }

    bool WorldController::CanBuyMule()
    {
        auto playerGroup = WorldScene::Get()->GetPlayerGroup();

        auto playerSettlement = playerGroup->GetCurrentSettlement();
        if(playerSettlement == nullptr)
            return false;

        auto playerMoney = playerGroup->GetMoney();

        auto price = playerSettlement->GetMulePrice();
        if(playerMoney < price)
            return false;

        return true;
    }

    void WorldController::BuyFood()
    {
        auto playerGroup = WorldScene::Get()->GetPlayerGroup();

        auto playerSettlement = playerGroup->GetCurrentSettlement();

        group::HumanMind::Get()->BuyFood();

        auto foodResource = playerSettlement->GetResource(settlement::ResourceTypes::FOOD);
        foodResource->Storage--;
    }

    group::Encounter * WorldController::GetPlayerBattle() const 
    {
        return WorldScene::Get()->GetPlayerGroup()->GetEncounter();
    }

    utility::PathData <WorldTile> WorldController::GetPlannedPath() const
    {
        return plannedPath;
    }
}