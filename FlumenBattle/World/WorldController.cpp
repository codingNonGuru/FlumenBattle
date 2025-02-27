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
#include "FlumenBattle/World/Tile/WorldMap.h"
#include "FlumenBattle/World/Tile/WorldTile.h"
#include "FlumenBattle/World/Group/Encounter.h"
#include "FlumenBattle/Battle/BattleState.h"
#include "FlumenBattle/World/Group/GroupCore.h"
#include "FlumenBattle/World/Group/GroupAction.h"
#include "FlumenBattle/World/Group/HumanMind.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/Utility/Pathfinder.h"
#include "FlumenBattle/World/Polity/HumanMind.h"
#include "FlumenBattle/WorldInterface.h"
#include "FlumenBattle/World/Group/GroupDynamics.h"

static const Float CAMERA_PAN_SPEED = 800.0f;

static const Float CAMERA_ZOOM_SPEED = 2.0f;

static const Float TRANSITION_TO_BATTLE_DELAY = 0.5f;

static const Float CAMERA_GRAB_SENSITIVITY = 1.7f;

static const Float CENTER_PLAYER_DURATION = 0.1f;

static Camera *camera = nullptr;

const int PLANNED_PATH_MAXIMUM_SIZE = 12;

static const auto TRAVEL_MODE_INPUT_KEY = SDL_Scancode::SDL_SCANCODE_T;

static const auto SCREEN_GRAB_INPUT_KEY = SDL_Scancode::SDL_SCANCODE_LALT;

static const auto RESOURCE_DISPLAY_KEY = InputHandler::Trigger{SDL_Scancode::SDL_SCANCODE_R, {InputHandler::CTRL}};

static const auto FOOD_DISPLAY_KEY = InputHandler::Trigger{SDL_Scancode::SDL_SCANCODE_F, {InputHandler::CTRL}};

static const auto TIMBER_DISPLAY_KEY = InputHandler::Trigger{SDL_Scancode::SDL_SCANCODE_T, {InputHandler::CTRL}};

static const auto FIBER_DISPLAY_KEY = InputHandler::Trigger{SDL_Scancode::SDL_SCANCODE_B, {InputHandler::CTRL}};

static const auto IMPROVEMENT_DISPLAY_KEY = InputHandler::Trigger{SDL_Scancode::SDL_SCANCODE_I, {InputHandler::CTRL}};

static const auto WORKER_DISPLAY_KEY = InputHandler::Trigger{SDL_Scancode::SDL_SCANCODE_W, {InputHandler::CTRL}};

static const auto BORDER_EXPAND_KEY = InputHandler::Trigger{SDL_Scancode::SDL_SCANCODE_E, {InputHandler::CTRL}};

static const auto SETTLE_MODE_KEY = InputHandler::Trigger{SDL_Scancode::SDL_SCANCODE_S, {InputHandler::CTRL}};

static const auto EXPLORE_MODE_KEY = InputHandler::Trigger{SDL_Scancode::SDL_SCANCODE_X, {InputHandler::CTRL}};

static const auto TILE_DEVELOP_MODE_KEY = InputHandler::Trigger{SDL_Scancode::SDL_SCANCODE_D, {InputHandler::CTRL}};

static const auto RULE_MENU_OPEN_KEY = InputHandler::Trigger{SDL_Scancode::SDL_SCANCODE_P};

namespace world
{
    WorldController::WorldController() {}

    void WorldController::Initialize()
    {
        WorldScene::Get()->OnPlayerEncounterInitiated += {this, &WorldController::HandlePlayerEncounterInitiated};

        WorldScene::Get()->OnPlayerEncounterFinished += {this, &WorldController::DisableEncounterMode};

        WorldScene::Get()->OnPlayerBattleStarted += {this, &WorldController::HandleBattleStarted};

        group::HumanMind::Get()->OnSettlementExited += {this, &WorldController::HandleSettlementExited};

        WorldInterface::Get()->OnCanvasHovered += {this, &WorldController::HandleWorldCanvasHovered};
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
        InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_M, {this, &WorldController::HandleMetalDisplayPressed});
        InputHandler::RegisterEvent(RESOURCE_DISPLAY_KEY, {this, &WorldController::HandleResourceDisplayPressed});
        InputHandler::RegisterEvent(FOOD_DISPLAY_KEY, {this, &WorldController::HandleFoodDisplayPressed});
        InputHandler::RegisterEvent(TIMBER_DISPLAY_KEY, {this, &WorldController::HandleTimberDisplayPressed});
        InputHandler::RegisterEvent(FIBER_DISPLAY_KEY, {this, &WorldController::HandleFiberDisplayPressed});
        InputHandler::RegisterEvent(IMPROVEMENT_DISPLAY_KEY, {this, &WorldController::HandleImprovementDisplayPressed});
        InputHandler::RegisterEvent(WORKER_DISPLAY_KEY, {this, &WorldController::HandleWorkerPlacePressed});
        InputHandler::RegisterEvent(RULE_MENU_OPEN_KEY, {this, &WorldController::HandleRuleMenuPressed});
        InputHandler::RegisterEvent(BORDER_EXPAND_KEY, {this, &WorldController::HandleBorderExpandPressed});
        InputHandler::RegisterEvent(SETTLE_MODE_KEY, {this, &WorldController::HandleSettleModePressed});
        InputHandler::RegisterEvent(EXPLORE_MODE_KEY, {this, &WorldController::HandleExploreModePressed});
        InputHandler::RegisterEvent(TILE_DEVELOP_MODE_KEY, {this, &WorldController::HandleTileDevelopModePressed});
        
        InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_1, {this, &WorldController::HandleCharacterSelected});
        InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_2, {this, &WorldController::HandleCharacterSelected});
        InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_3, {this, &WorldController::HandleCharacterSelected});
        InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_4, {this, &WorldController::HandleCharacterSelected});
        InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_5, {this, &WorldController::HandleCharacterSelected});
        InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_6, {this, &WorldController::HandleCharacterSelected});
        InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_I, {this, &WorldController::HandleInventoryPressed});

        InputHandler::RegisterContinualEvent(TRAVEL_MODE_INPUT_KEY, {this, &WorldController::HandleTravelPressed}, {this, &WorldController::HandleTravelReleased});
        InputHandler::RegisterContinualEvent(SCREEN_GRAB_INPUT_KEY, {this, &WorldController::HandleGrabPressed}, {this, &WorldController::HandleGrabReleased});

        InputHandler::RegisterDoubleClickEvent({this, &WorldController::HandleCenterPlayerPressed});
        
        //InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_C, {this, &WorldController::HandleColonizationSwitch});

        camera = RenderManager::GetCamera(Cameras::WORLD);
        camera->EnableDynamicZooming();

        camera->SetTarget(WorldScene::Get()->GetPlayerGroup()->GetVisualPosition());
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

        auto polityController = polity::HumanMind::Get();
        polityController->DisableInput();

        InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_SPACE);
        InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_COMMA);
        InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_PERIOD);
    }

    void WorldController::EnableHardInput()
    {
        auto controller = group::HumanMind::Get();
        controller->EnableInput();

        auto polityController = polity::HumanMind::Get();
        polityController->EnableInput();

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
                bool isMouseOverTile = PickHandler::CheckCollision(camera, mesh, Position3(nearbyTile->Position, 0.0f), tile::WorldMap::WORLD_TILE_SIZE); 
                if(isMouseOverTile)
                {
                    hoveredTile = nearbyTile;
                    return;
                }   
            }
        }
    }

    bool willCheckTileSelection = false;

    void WorldController::HandleWorldCanvasHovered()
    {
        willCheckTileSelection = true;
    }

    static utility::PathData <tile::WorldTile> plannedPath;

    void WorldController::Update()
    {
        if(WorldInterface::Get()->IsAnyMajorMenuEnabled() == true)
        {
            CancelActiveModes();
        }

        if(willCheckTileSelection == true)
        {
            CheckTileSelection();

            willCheckTileSelection = false;
        }
        else
        {
            hoveredTile = nullptr;
        }

        auto zoomSpeed = CAMERA_ZOOM_SPEED * Time::GetDelta();

        if(InputHandler::GetMouse().ScrollUp_)
        {
            camera->ZoomDynamically(zoomSpeed);
        }
        else if(InputHandler::GetMouse().ScrollDown_)
        {
            camera->ZoomDynamically(-zoomSpeed);
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

        plannedPath = utility::Pathfinder <tile::WorldTile>::Get()->FindPathAsGeneric(hoveredTile, playerLocation, PLANNED_PATH_MAXIMUM_SIZE - 4);
    }

    void WorldController::HandleBattleStarted()
    {
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

    void WorldController::HandleCenterPlayerPressed()
    {
        camera->SetTarget(WorldScene::Get()->GetPlayerGroup()->GetVisualPosition(), CENTER_PLAYER_DURATION);
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

    void WorldController::HandleTravelPressed()
    {
        if(WorldInterface::Get()->IsAnyMajorMenuEnabled() == true)
            return;

        isTravelPlanActive = true;

        WorldScene::Get()->GetPlayerSettlement()->KillPopulation();
    }

    void WorldController::HandleTravelReleased()
    {
        isTravelPlanActive = false;
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
        if(isResourceDisplayActive == true && shouldResourceDisplayIncludeBonus == true)
        {
            isResourceDisplayActive = false;
        }
        else if(isResourceDisplayActive == true && shouldResourceDisplayIncludeBonus == false)
        {
            shouldResourceDisplayIncludeBonus = true;
        }
        else if(isResourceDisplayActive == false)
        {
            isResourceDisplayActive = true;

            shouldResourceDisplayIncludeBonus = false;
        }

        OnResourceDisplayPressed.Invoke();
    }

    void WorldController::HandleMetalDisplayPressed()
    {
        if(isMetalDisplayActive)
        {
            isMetalDisplayActive = false;
        }
        else
        {
            isMetalDisplayActive = true;
        }
    }

    void WorldController::HandleFoodDisplayPressed()
    {
        if(isFoodDisplayActive)
        {
            isFoodDisplayActive = false;
        }
        else
        {
            isFoodDisplayActive = true;

            isTimberDisplayActive = false;

            isFiberDisplayActive = false;
        }
    }

    void WorldController::HandleTimberDisplayPressed()
    {
        if(isTimberDisplayActive)
        {
            isTimberDisplayActive = false;
        }
        else
        {
            isTimberDisplayActive = true;

            isFoodDisplayActive = false;

            isFiberDisplayActive = false;
        }
    }

    void WorldController::HandleFiberDisplayPressed()
    {
        if(isFiberDisplayActive)
        {
            isFiberDisplayActive = false;
        }
        else
        {
            isFiberDisplayActive = true;

            isFoodDisplayActive = false;

            isTimberDisplayActive = false;
        }
    }

    void WorldController::HandleImprovementDisplayPressed()
    {
        if(isImprovementDisplayActive)
        {
            isImprovementDisplayActive = false;
        }
        else
        {
            isImprovementDisplayActive = true;
        }
    }

    void WorldController::HandleWorkerPlacePressed()
    {
        if(isBorderExpandActive == true)
            return;

        if(isSettleModeActive == true)
            return;

        if(WorldInterface::Get()->IsAnyMajorMenuEnabled() == true)
            return;

        if(WorldScene::Get()->GetPlayerPolity() == nullptr)
            return;

        if(WorldScene::Get()->GetPlayerSettlement() == nullptr)
            return;

        if(isWorkerPlaceModeActive)
        {
            isWorkerPlaceModeActive = false;
        }
        else
        {
            isWorkerPlaceModeActive = true;

            onWorkerPlaceModeEntered.Invoke();
        }

        OnWorkerPlaceModeToggled.Invoke();
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

    void WorldController::HandleRuleMenuPressed()
    {
        onRuleMenuPressed.Invoke();
    }

    void WorldController::HandleBorderExpandPressed()
    {
        if(isWorkerPlaceModeActive == true)
            return;

        if(isSettleModeActive == true)
            return;

        if(isExploreModeActive == true)
            return;

        if(isTileDevelopModeActive == true)
            return;

        if(WorldInterface::Get()->IsAnyMajorMenuEnabled() == true)
            return;

        if(WorldScene::Get()->GetPlayerSettlement() == nullptr)
            return;

        if(WorldScene::Get()->GetPlayerSettlement()->GetPolity() != WorldScene::Get()->GetPlayerPolity())
            return;

        if(isBorderExpandActive == true)
        {
            isBorderExpandActive = false;
        }
        else
        {
            isBorderExpandActive = true;
        }
    }

    void WorldController::HandleSettleModePressed()
    {
        if(isWorkerPlaceModeActive == true)
            return;

        if(isBorderExpandActive == true)
            return;

        if(isExploreModeActive == true)
            return;

        if(isTileDevelopModeActive == true)
            return;

        if(WorldInterface::Get()->IsAnyMajorMenuEnabled() == true)
            return;

        if(WorldScene::Get()->GetPlayerSettlement() == nullptr)
            return;

        if(WorldScene::Get()->GetPlayerSettlement()->GetPolity() != WorldScene::Get()->GetPlayerPolity())
            return;

        if(isSettleModeActive == true)
        {
            isSettleModeActive = false;
        }
        else
        {
            isSettleModeActive = true;
        }
    }

    void WorldController::HandleExploreModePressed()
    {
        if(isWorkerPlaceModeActive == true)
            return;

        if(isBorderExpandActive == true)
            return;

        if(isSettleModeActive == true)
            return;

        if(isTileDevelopModeActive == true)
            return;

        if(WorldInterface::Get()->IsAnyMajorMenuEnabled() == true)
            return;

        if(WorldScene::Get()->GetPlayerSettlement() == nullptr)
            return;

        if(WorldScene::Get()->GetPlayerSettlement()->GetPolity() != WorldScene::Get()->GetPlayerPolity())
            return;

        if(isExploreModeActive == true)
        {
            isExploreModeActive = false;
        }
        else
        {
            isExploreModeActive = true;
        }

        OnExploreModeToggled.Invoke();
    }

    void WorldController::HandleTileDevelopModePressed()
    {
        if(isWorkerPlaceModeActive == true)
            return;

        if(isBorderExpandActive == true)
            return;

        if(isSettleModeActive == true)
            return;

        if(isExploreModeActive == true)
            return;

        if(WorldInterface::Get()->IsAnyMajorMenuEnabled() == true)
            return;

        if(WorldScene::Get()->GetPlayerSettlement() == nullptr)
            return;

        if(WorldScene::Get()->GetPlayerSettlement()->GetPolity() != WorldScene::Get()->GetPlayerPolity())
            return;

        if(isTileDevelopModeActive == true)
        {
            isTileDevelopModeActive = false;
        }
        else
        {
            isTileDevelopModeActive = true;

            OnTileDevelopModeEnabled.Invoke();
        }

        OnTileDevelopModeToggled.Invoke();
    }

    void WorldController::HandleSettlementExited()
    {
        isWorkerPlaceModeActive = false;

        isImprovementDisplayActive = false;

        isBorderExpandActive = false;

        isSettleModeActive = false;

        isExploreModeActive = false;

        isTileDevelopModeActive = false;
    }

    void WorldController::Disable()
    {
        DisableHardInput();

        InputHandler::UnregisterContinualEvent(SDL_Scancode::SDL_SCANCODE_UP);
        InputHandler::UnregisterContinualEvent(SDL_Scancode::SDL_SCANCODE_DOWN);
        InputHandler::UnregisterContinualEvent(SDL_Scancode::SDL_SCANCODE_RIGHT);
        InputHandler::UnregisterContinualEvent(SDL_Scancode::SDL_SCANCODE_LEFT);
        InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_M);
        InputHandler::UnregisterEvent(RESOURCE_DISPLAY_KEY);
        InputHandler::UnregisterEvent(FOOD_DISPLAY_KEY);
        InputHandler::UnregisterEvent(TIMBER_DISPLAY_KEY);
        InputHandler::UnregisterEvent(FIBER_DISPLAY_KEY);
        InputHandler::UnregisterEvent(IMPROVEMENT_DISPLAY_KEY);
        InputHandler::UnregisterEvent(WORKER_DISPLAY_KEY);
        InputHandler::UnregisterEvent(RULE_MENU_OPEN_KEY);
        InputHandler::UnregisterEvent(BORDER_EXPAND_KEY);
        InputHandler::UnregisterEvent(SETTLE_MODE_KEY);
        InputHandler::UnregisterEvent(EXPLORE_MODE_KEY);
        InputHandler::UnregisterEvent(TILE_DEVELOP_MODE_KEY);
        InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_1);
        InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_2);
        InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_3);
        InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_4);
        InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_5);
        InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_6);
        InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_I);

        InputHandler::UnregisterContinualEvent(SCREEN_GRAB_INPUT_KEY);
        InputHandler::UnregisterContinualEvent(TRAVEL_MODE_INPUT_KEY);

        InputHandler::UnregisterDoubleClickEvent({this, &WorldController::HandleCenterPlayerPressed});
    }

    bool WorldController::CanAttackGarrison()
    {
        static const auto playerGroup = WorldScene::Get()->GetPlayerGroup();

        auto playerSettlement = playerGroup->GetCurrentSettlement();
        if(playerSettlement == nullptr)
            return false;

        if(playerSettlement->IsDefended() == false)
            return false;

        if(playerGroup->GetDomain() == playerSettlement->GetPolity())
            return false;

        return true;
    }

    bool WorldController::CanConquerSettlement()
    {
        static const auto playerGroup = WorldScene::Get()->GetPlayerGroup();

        auto playerSettlement = playerGroup->GetCurrentSettlement();
        if(playerSettlement == nullptr)
            return false;

        if(playerSettlement->IsDefended() == true)
            return false;

        if(playerGroup->GetDomain() == playerSettlement->GetPolity())
            return false;

        return true;
    }

    bool WorldController::CanLootSettlement()
    {
        static const auto playerGroup = WorldScene::Get()->GetPlayerGroup();

        auto playerSettlement = playerGroup->GetCurrentSettlement();
        if(playerSettlement == nullptr)
            return false;

        if(playerSettlement->IsDefended() == true)
            return false;

        if(playerSettlement->GetStandingBuildingCount() == 0)
            return false;

        if(playerGroup->GetDomain() == playerSettlement->GetPolity())
            return false;

        return true;
    }

    bool WorldController::CanPillageSettlement()
    {
        static const auto playerGroup = WorldScene::Get()->GetPlayerGroup();

        auto playerSettlement = playerGroup->GetCurrentSettlement();
        if(playerSettlement == nullptr)
            return false;

        if(playerSettlement->IsDefended() == true)
            return false;

        if(playerSettlement->GetTotalBuildingCount() == 0)
            return false;

        if(playerGroup->GetDomain() == playerSettlement->GetPolity())
            return false;

        return true;
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

    void WorldController::AttackGarrison()
    {
        static const auto playerGroup = WorldScene::Get()->GetPlayerGroup();

        auto playerSettlement = playerGroup->GetCurrentSettlement();

        auto localGroups = WorldScene::Get()->GetGroupsInTile(playerSettlement->GetLocation());
        
        group::GroupCore *garrison = nullptr;
        for(auto &group : localGroups.Groups)
        {
            if(group->GetClass() != group::GroupClasses::GARRISON)
                continue;

            garrison = group;
            break;
        }

        WorldScene::Get()->InitiateEncounter(playerGroup, garrison);
    }

    void WorldController::LootSettlement()
    {
        static const auto playerGroup = WorldScene::Get()->GetPlayerGroup();

        auto playerSettlement = playerGroup->GetCurrentSettlement();

        playerGroup->SelectAction(group::GroupActions::LOOT_SETTLEMENT, {playerSettlement});
    }

    void WorldController::PillageSettlement()
    {
        static const auto playerGroup = WorldScene::Get()->GetPlayerGroup();

        auto playerSettlement = playerGroup->GetCurrentSettlement();

        playerGroup->SelectAction(group::GroupActions::PILLAGE_SETTLEMENT, {playerSettlement});
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

    utility::PathData <tile::WorldTile> WorldController::GetPlannedPath() const
    {
        return plannedPath;
    }

    void WorldController::CancelActiveModes()
    {
        isWorkerPlaceModeActive = false;

        isTravelPlanActive = false;

        isBorderExpandActive = false;

        isSettleModeActive = false;

        isExploreModeActive = false;

        isTileDevelopModeActive = false;
    }
}