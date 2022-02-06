#include "FlumenEngine/Core/InputHandler.hpp"
#include "FlumenEngine/Utility/PickHandler.h"
#include "FlumenEngine/Render/MeshManager.hpp"

#include "FlumenBattle/HumanController.h"
#include "FlumenBattle/BattleController.h"
#include "FlumenBattle/Character.h"
#include "FlumenBattle/Combatant.h"
#include "FlumenBattle/BattleScene.h"
#include "FlumenBattle/BattleMap.h"
#include "FlumenBattle/BattleTile.h"

static BattleController *battleController = nullptr;

static Combatant *selectedCombatant = nullptr;

static BattleScene *battleScene = nullptr;

static Camera *camera = nullptr;

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
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_SPACE, {this, &HumanController::HandleSpacePressed});
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_1, {this, &HumanController::HandleOnePressed});
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_2, {this, &HumanController::HandleTwoPressed});
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_3, {this, &HumanController::HandleThreePressed});
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_4, {this, &HumanController::HandleFourPressed});
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_M, {this, &HumanController::HandleMPressed});
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_T, {this, &HumanController::HandleTPressed});
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_A, {this, &HumanController::HandleAPressed});
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
        bool isMouseOverTile = PickHandler::CheckCollision(camera, mesh, Position3(tile->Position, 0.0f), 33.0f); 
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
        }
    }
    else
    {
        auto currentIndex = character->GetSelectedActionIndex();
        battleController->SelectAction(actionIndex);
        
        if(actionIndex != currentIndex)
        {
            isInitiatingTargeting = false;
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
    }
    else
    {
        isInitiatingTargeting = true;
        isInitiatingMove = false;
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
}

