#include "FlumenEngine/Core/InputHandler.hpp"
#include "FlumenEngine/Utility/PickHandler.h"
#include "FlumenEngine/Render/MeshManager.hpp"
#include "FlumenEngine/Render/Camera.hpp"

#include "FlumenBattle/Battle/HumanController.h"
#include "FlumenBattle/Battle/BattleController.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/Battle/Combatant.h"
#include "FlumenBattle/Battle/BattleScene.h"
#include "FlumenBattle/Battle/BattleMap.h"
#include "FlumenBattle/Battle/BattleTile.h"
#include "FlumenBattle/Utility/Pathfinder.h"

using namespace battle;

static BattleController *battleController = nullptr;

static Combatant *selectedCombatant = nullptr;

static BattleScene *battleScene = nullptr;

static Camera *camera = nullptr;

static utility::PathData <BattleTile> hoveredPathData;

HumanController::HumanController()
{
    OnTargetInitiated = new Delegate();

    OnTargetAbandoned = new Delegate();
}

void HumanController::Initialize()
{
    isInitiatingMove = false;

    isInitiatingTargeting = false;

    hoveredTile = nullptr;

    battleController = BattleController::Get();
    selectedCombatant = battleController->selectedCombatant;

    battleScene = BattleScene::Get();

    camera = battleScene->GetCamera();

    battleScene->OnUpdate += {this, &HumanController::HandleSceneUpdate};
}

static const SDL_Scancode keys[] = {SDL_Scancode::SDL_SCANCODE_1, SDL_Scancode::SDL_SCANCODE_2, SDL_Scancode::SDL_SCANCODE_3, SDL_Scancode::SDL_SCANCODE_4};

void HumanController::EnablePlayerInput()
{
    static const Event numberEvents[] = {
        {this, &HumanController::HandleNumberPressed <0>}, 
        {this, &HumanController::HandleNumberPressed <1>},
        {this, &HumanController::HandleNumberPressed <2>},
        {this, &HumanController::HandleNumberPressed <3>}
    };

    static const Event ctrlNumberEvents[] = {
        {this, &HumanController::HandleCtrlNumberPressed <0>}, 
        {this, &HumanController::HandleCtrlNumberPressed <1>},
        {this, &HumanController::HandleCtrlNumberPressed <2>},
        {this, &HumanController::HandleCtrlNumberPressed <3>}
    };
    
    for(int i = 0; i < std::size(keys); ++i)
    {
        InputHandler::RegisterEvent(keys[i], numberEvents[i]);    
        InputHandler::RegisterEvent({keys[i], {InputHandler::SupportKeys::CTRL}}, ctrlNumberEvents[i]);    
    }

    InputHandler::OnInputUpdate += {this, &HumanController::CheckTileSelection};
    InputHandler::OnLeftMouseClick += {this, &HumanController::CheckCharacterMovement};
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_SPACE, {this, &HumanController::HandleSpacePressed});
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_M, {this, &HumanController::HandleMPressed});
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_T, {this, &HumanController::HandleTPressed});
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_A, {this, &HumanController::HandleAPressed});
}

void HumanController::DisablePlayerInput()
{
    for(int i = 0; i < std::size(keys); ++i)
    {
        InputHandler::UnregisterEvent(keys[i]);    
        InputHandler::UnregisterEvent({keys[i], {InputHandler::SupportKeys::CTRL}});
    }

    InputHandler::OnInputUpdate -= {this, &HumanController::CheckTileSelection};
    InputHandler::OnLeftMouseClick -= {this, &HumanController::CheckCharacterMovement};
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_SPACE);
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_M);
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_T);
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_A);
}

void HumanController::CheckCharacterMovement()
{
    if(isInitiatingMove == true)
    {
        isInitiatingMove = false;

        if(hoveredTile->IsObstacle)
            return;

        battleController->TargetTileForMovement(hoveredTile);

        battleController->Move(hoveredPathData);
    }
    else if(isInitiatingTargeting == true)
    {
        TargetTile(hoveredTile);
    }
}

void HumanController::CheckTileSelection()
{
    auto mesh = MeshManager::GetMesh("Hex"); 

    auto battleMap = battleScene->GetBattleMap();
    for(auto tile = battleMap->tiles.GetStart(); tile != battleMap->tiles.GetEnd(); ++tile) 
    {
        bool isMouseOverTile = PickHandler::CheckCollision(camera, mesh, Position3(tile->Position, 0.0f), BattleMap::TILE_SIZE * 1.01f); 
        if(isMouseOverTile)
        {
            hoveredTile = tile;
            return;
        }
    }

    hoveredTile = nullptr;
}

Position2 HumanController::GetHoveredPosition() const
{
    Position2 mousePosition = InputHandler::GetMousePosition();

    return camera->GetWorldPosition(mousePosition);
}

void HumanController::HandleSpacePressed()
{
    auto battleController = BattleController::Get();
    auto selectedCombatant = battleController->selectedCombatant;

    battleController->EndTurn();
}

template <int N>
void HumanController::HandleNumberPressed()
{
    ChangeActionSelection(N, false);
}

template <int N>
void HumanController::HandleCtrlNumberPressed()
{
    ChangeActionSelection(N, true);
}

void HumanController::ChangeActionSelection(Integer actionIndex, bool isSubaction)
{
    auto battleController = BattleController::Get();
    auto selectedCombatant = battleController->selectedCombatant;

    if(selectedCombatant == nullptr)
        return;

    auto character = selectedCombatant->GetCharacter();

    if(isSubaction == true)
    {
        auto currentIndex = character->GetSelectedSubactionIndex();
        battleController->SelectSubaction(actionIndex);

        if(actionIndex != currentIndex)
        {
            isInitiatingTargeting = false;

            OnTargetAbandoned->Invoke();
        }
    }
    else
    {
        auto currentIndex = character->GetSelectedActionIndex();
        battleController->SelectAction(actionIndex);
        
        if(actionIndex != currentIndex)
        {
            isInitiatingTargeting = false;

            OnTargetAbandoned->Invoke();
        }
    }
}

void HumanController::HandleMPressed()
{
    if(BattleController::Get()->IsAnimationOngoing() == true)
        return;
        
    if(isInitiatingMove)
    {
        isInitiatingMove = false;
    }
    else
    {
        isInitiatingMove = true;
        isInitiatingTargeting = false;
    }
}

void HumanController::HandleTPressed()
{
    auto battleController = BattleController::Get();
    auto selectedCombatant = battleController->selectedCombatant;

    if(selectedCombatant->CanTarget() == false)
        return;

    if(isInitiatingTargeting == true)
    {
        isInitiatingTargeting = false;

        OnTargetAbandoned->Invoke();
    }
    else
    {
        isInitiatingTargeting = true;
        isInitiatingMove = false;

        OnTargetInitiated->Invoke();
    }
}

void HumanController::HandleAPressed()
{
    auto battleController = BattleController::Get();
    auto selectedCombatant = battleController->selectedCombatant;

    battleController->Act();
}

void HumanController::HandleSceneUpdate()
{
    if(hoveredTile == nullptr)
        return;

    if(isInitiatingMove == true)
    {
        if(hoveredTile->IsObstacle == true || hoveredTile->Combatant != nullptr)
        {
            hoveredPathData.Tiles.Clear();
            return;
        }

        auto battleController = BattleController::Get();
        auto selectedCombatant = battleController->GetSelectedCombatant();

        if(hoveredTile->GetDistanceTo(*selectedCombatant->GetTile()) <= MAXIMUM_PATH_LENGTH)
        {
            hoveredPathData = utility::Pathfinder <BattleTile>::Get()->FindPathAsGeneric(hoveredTile, selectedCombatant->GetTile(), MAXIMUM_PATH_LENGTH - 3);
        }
    }
}

void HumanController::TargetCombatant(Combatant *target)
{
    if(isInitiatingTargeting == false)
        return;

    battleController->TargetCombatant(target);

    isInitiatingTargeting = false;

    OnTargetAbandoned->Invoke();
}

void HumanController::TargetTile(BattleTile *target)
{
    if(isInitiatingTargeting == false)
        return;

    battleController->TargetTile(target);

    isInitiatingTargeting = false;

    OnTargetAbandoned->Invoke();
}

const utility::PathData <BattleTile> &HumanController::GetHoveredPath()
{
    return hoveredPathData;
}

