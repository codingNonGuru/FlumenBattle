#include "FlumenEngine/Core/InputHandler.hpp"
#include "FlumenEngine/Utility/PickHandler.h"
#include "FlumenEngine/Render/MeshManager.hpp"

#include "FlumenBattle/Battle/HumanController.h"
#include "FlumenBattle/Battle/BattleController.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/Battle/Combatant.h"
#include "FlumenBattle/Battle/BattleScene.h"
#include "FlumenBattle/Battle/BattleMap.h"
#include "FlumenBattle/Battle/BattleTile.h"

using namespace battle;

static BattleController *battleController = nullptr;

static Combatant *selectedCombatant = nullptr;

static BattleScene *battleScene = nullptr;

static Camera *camera = nullptr;

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
}

void HumanController::EnablePlayerInput()
{
    InputHandler::OnInputUpdate += {this, &HumanController::CheckTileSelection};
    InputHandler::OnLeftMouseClick += {this, &HumanController::CheckCharacterMovement};
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_SPACE, {this, &HumanController::HandleSpacePressed});
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_1, {this, &HumanController::HandleOnePressed});
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_2, {this, &HumanController::HandleTwoPressed});
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_3, {this, &HumanController::HandleThreePressed});
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_4, {this, &HumanController::HandleFourPressed});
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_M, {this, &HumanController::HandleMPressed});
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_T, {this, &HumanController::HandleTPressed});
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_A, {this, &HumanController::HandleAPressed});
}

void HumanController::DisablePlayerInput()
{
    InputHandler::OnInputUpdate -= {this, &HumanController::CheckTileSelection};
    InputHandler::OnLeftMouseClick -= {this, &HumanController::CheckCharacterMovement};
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_SPACE);
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_1);
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_2);
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_3);
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_4);
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_M);
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_T);
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_A);
}

void HumanController::CheckCharacterMovement()
{
    if(isInitiatingMove == false)
        return;

    isInitiatingMove = false;

    if(hoveredTile->IsObstacle)
        return;

    battleController->TargetTile(hoveredTile);

    battleController->Move();
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

void HumanController::HandleSpacePressed()
{
    auto battleController = BattleController::Get();
    auto selectedCombatant = battleController->selectedCombatant;

    battleController->EndTurn();
}

void HumanController::HandleOnePressed()
{
    ChangeActionSelection(0);
}

void HumanController::HandleTwoPressed()
{
    ChangeActionSelection(1);
}

void HumanController::HandleThreePressed()
{
    ChangeActionSelection(2);
}

void HumanController::HandleFourPressed()
{
    ChangeActionSelection(3);
}

void HumanController::ChangeActionSelection(Integer actionIndex)
{
    auto battleController = BattleController::Get();
    auto selectedCombatant = battleController->selectedCombatant;

    if(selectedCombatant == nullptr)
        return;

    auto character = selectedCombatant->GetCharacter();

    if(InputHandler::IsPressed(SDL_Scancode::SDL_SCANCODE_LCTRL))
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

    if(isInitiatingTargeting)
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

void HumanController::TargetCombatant(Combatant *target)
{
    if(isInitiatingTargeting == false)
        return;

    battleController->TargetCombatant(target);

    isInitiatingTargeting = false;

    OnTargetAbandoned->Invoke();
}

