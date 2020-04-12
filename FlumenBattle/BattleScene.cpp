#include "FlumenCore/Time.hpp"

#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Core/InputHandler.hpp"
#include "FlumenEngine/Utility/PickHandler.h"
#include "FlumenEngine/Render/MeshManager.hpp"
#include "FlumenEngine/Render/Camera.hpp"

#include "FlumenBattle/BattleScene.h"
#include "FlumenBattle/CharacterModel.h"
#include "FlumenBattle/GroupFactory.h"
#include "FlumenBattle/Group.h"
#include "FlumenBattle/CharacterInfo.h"
#include "FlumenBattle/Character.h"
#include "FlumenBattle/BattleTileModel.h"
#include "FlumenBattle/BattleMap.h"

static Camera* camera = nullptr;

static const Float CAMERA_PAN_SPEED = 350.0f;

static const Float CAMERA_ZOOM_SPEED = 5.0f;

static const Length BATTLE_MAP_SIZE = 35;

static const Length MAXIMUM_GROUP_COUNT = 16;

BattleScene::BattleScene() {}

void BattleScene::Initialize()
{
    battleMap = new BattleMap(BATTLE_MAP_SIZE);

    groups.Initialize(MAXIMUM_GROUP_COUNT);
    GroupFactory::Create(groups);
    GroupFactory::Create(groups);

    characterModel = new CharacterModel();
    characterModel->Initialize();

    battleTileModel = new BattleTileModel();
    battleTileModel->Initialize();

    InputHandler::OnInputUpdate.Add(this, &CheckTileSelection);
    InputHandler::OnRightMouseClick.Add(this, &CheckCharacterMovement);
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_SPACE, this, &HandleSpacePressed);
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_1, this, &HandleOnePressed);
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_2, this, &HandleTwoPressed);

    camera = RenderManager::GetCamera(Cameras::BATTLE);

    selectedCharacter = nullptr;

    DetermineTurnOrder();
}

void BattleScene::DetermineTurnOrder()
{
    turnOrder.Initialize(256);

    for(auto group = groups.GetStart(); group != groups.GetEnd(); ++group)
    {
        for(auto character = group->characters.GetStart(); character != group->characters.GetEnd(); ++character)
        {
            auto initiative = character->RollInitiative();
            *turnOrder.Allocate() = {character, initiative};
        }
    }

    turnOrder.Sort();

    turn = turnOrder.GetStart();

    SelectCharacter(turn->Character);

    selectedCharacter->StartTurn();
}

void BattleScene::CheckCharacterMovement()
{
    if(hoveredTile == nullptr)
        return;

    if(selectedCharacter == nullptr)
        return;

    if(turn->Character != selectedCharacter)
        return;

    if(InputHandler::GetMouse().CurrentRight_)
    {
        selectedCharacter->Move(hoveredTile);
    }
}

void BattleScene::HandleSpacePressed()
{
    while(true)
    {
        turn++;
        if(turn == turnOrder.GetEnd())
        {
            turn = turnOrder.GetStart();
        }

        if(turn->Character->IsAlive())
        {
            break;
        }

        if(turn->Character->IsSavingAgainstDeath())
        {
            turn->Character->SaveAgainstDeath();
        }
    }

    SelectCharacter(turn->Character);

    selectedCharacter->StartTurn();
}

void BattleScene::HandleOnePressed()
{
    if(selectedCharacter == nullptr)
        return;

    if(InputHandler::IsPressed(SDL_Scancode::SDL_SCANCODE_LCTRL))
    {
        if(selectedCharacter->SelectActionOption(0))
        {
            OnSubactionSelected.Invoke();
        }
    }
    else
    {
        if(selectedCharacter->SelectAction(0))
        {
            OnActionSelected.Invoke();
        }
    }
}

void BattleScene::HandleTwoPressed()
{
    if(selectedCharacter == nullptr)
        return;

    if(InputHandler::IsPressed(SDL_Scancode::SDL_SCANCODE_LCTRL))
    {
        if(selectedCharacter->SelectActionOption(1))
        {
            OnSubactionSelected.Invoke();
        }
    }
    else
    {
        if(selectedCharacter->SelectAction(1))
        {
            OnActionSelected.Invoke();
        }
    }
}

void BattleScene::Update()
{
    auto panSpeed = CAMERA_PAN_SPEED * Time::GetDelta();

    if(InputHandler::IsPressed(SDL_SCANCODE_UP))
    {
        camera->Translate(Direction3(0.0f, -panSpeed, 0.0f));
    }
    else if(InputHandler::IsPressed(SDL_SCANCODE_DOWN))
    {
        camera->Translate(Direction3(0.0f, panSpeed, 0.0f));
    }

    if(InputHandler::IsPressed(SDL_SCANCODE_LEFT))
    {
        camera->Translate(Direction3(-panSpeed, 0.0f, 0.0f));
    }
    else if(InputHandler::IsPressed(SDL_SCANCODE_RIGHT))
    {
        camera->Translate(Direction3(panSpeed, 0.0f, 0.0f));
    }

    auto zoomSpeed = CAMERA_ZOOM_SPEED * Time::GetDelta();

    if(InputHandler::GetMouse().ScrollUp_)
    {
        camera->Zoom(1.0f - zoomSpeed);
    }
    else if(InputHandler::GetMouse().ScrollDown_)
    {
        camera->Zoom(1.0f + zoomSpeed);
    }
}

void BattleScene::Render()
{
    battleTileModel->Render();
}

void BattleScene::CheckTileSelection()
{
    auto mesh = MeshManager::GetMesh("Hex");
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

void BattleScene::SelectCharacter(Character* newlySelected)
{
    if(selectedCharacter != nullptr)
    {
        selectedCharacter->Deselect();
    }

    selectedCharacter = newlySelected;

    if(selectedCharacter != nullptr)
    {
        selectedCharacter->Select();
    }

    OnCharacterSelected.Invoke();
}

void BattleScene::TargetCharacter(Character* target)
{
    if(selectedCharacter == nullptr)
        return;

    if(target == nullptr)
        return;

    if(turn->Character != selectedCharacter)
        return;

    if(selectedCharacter->CanAct(*target) == false)
        return;

    lastActionData = selectedCharacter->Act(*target);

    OnCharacterActed.Invoke();
}

bool BattleScene::IsCharactersTurn(Character* character) const
{
    return character == turn->Character;
}