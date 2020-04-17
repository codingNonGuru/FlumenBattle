#include "FlumenEngine/Core/InputHandler.hpp"
#include "FlumenEngine/Utility/PickHandler.h"
#include "FlumenEngine/Render/MeshManager.hpp"

#include "FlumenBattle/HumanController.h"
#include "FlumenBattle/BattleController.h"
#include "FlumenBattle/Character.h"
#include "FlumenBattle/BattleScene.h"
#include "FlumenBattle/BattleMap.h"
#include "FlumenBattle/BattleTile.h"

static BattleController *battleController = nullptr;

static Character *selectedCharacter = nullptr;

static BattleScene *battleScene = nullptr;

static Camera *camera = nullptr;

void HumanController::Initialize()
{
    isInitiatingMove = false;

    isInitiatingTargeting = false;

    hoveredTile = nullptr;

    battleController = BattleController::Get();
    selectedCharacter = battleController->selectedCharacter;

    battleScene = BattleScene::Get();

    camera = battleScene->GetCamera();
}

void HumanController::EnablePlayerInput()
{
    InputHandler::OnInputUpdate.Add(this, &CheckTileSelection);
    InputHandler::OnLeftMouseClick.Add(this, &CheckCharacterMovement);
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_SPACE, this, &HandleSpacePressed);
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_1, this, &HandleOnePressed);
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_2, this, &HandleTwoPressed);
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_3, this, &HandleThreePressed);
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_4, this, &HandleFourPressed);
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_M, this, &HandleMPressed);
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_T, this, &HandleTPressed);
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_A, this, &HandleAPressed);
}

void HumanController::DisablePlayerInput()
{
    InputHandler::OnInputUpdate.Remove(this, &CheckTileSelection);
    InputHandler::OnLeftMouseClick.Remove(this, &CheckCharacterMovement);
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_SPACE, this, &HandleSpacePressed);
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_1, this, &HandleOnePressed);
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_2, this, &HandleTwoPressed);
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_3, this, &HandleThreePressed);
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_4, this, &HandleFourPressed);
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_M, this, &HandleMPressed);
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_T, this, &HandleTPressed);
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_A, this, &HandleAPressed);
}

void HumanController::CheckCharacterMovement()
{
    if(isInitiatingMove == false)
        return;

    isInitiatingMove = false;

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
    auto selectedCharacter = battleController->selectedCharacter;

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
    auto selectedCharacter = battleController->selectedCharacter;

    if(selectedCharacter == nullptr)
        return;

    if(InputHandler::IsPressed(SDL_Scancode::SDL_SCANCODE_LCTRL))
    {
        auto currentIndex = selectedCharacter->GetSelectedSubactionIndex();
        battleController->SelectSubaction(actionIndex);

        if(actionIndex != currentIndex)
        {
            isInitiatingTargeting = false;
        }
    }
    else
    {
        auto currentIndex = selectedCharacter->GetSelectedActionIndex();
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
    auto selectedCharacter = battleController->selectedCharacter;

    if(selectedCharacter->CanTarget() == false)
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
    auto selectedCharacter = battleController->selectedCharacter;

    battleController->Act();
}

void HumanController::TargetCharacter(Character *target)
{
    if(isInitiatingTargeting == false)
        return;

    battleController->TargetCharacter(target);

    isInitiatingTargeting = false;
}

