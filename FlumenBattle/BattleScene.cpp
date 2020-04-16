#include "FlumenCore/Time.hpp"
#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Core/InputHandler.hpp"
#include "FlumenEngine/Render/MeshManager.hpp"
#include "FlumenEngine/Render/Camera.hpp"

#include "FlumenBattle/BattleScene.h"
#include "FlumenBattle/BattleInterface.h"
#include "FlumenBattle/GroupFactory.h"
#include "FlumenBattle/Group.h"
#include "FlumenBattle/CharacterInfo.h"
#include "FlumenBattle/Character.h"
#include "FlumenBattle/BattleTileModel.h"
#include "FlumenBattle/BattleMap.h"
#include "FlumenBattle/BattleTile.h"

static Camera* camera = nullptr;

static const Float CAMERA_PAN_SPEED = 1500.0f;

static const Float CAMERA_ZOOM_SPEED = 10.0f;

static const Length BATTLE_MAP_SIZE = 45;

static const Length MAXIMUM_GROUP_COUNT = 16;

BattleScene* BattleScene::instance = nullptr;

BattleScene::BattleScene() {}

void BattleScene::Initialize()
{
    battleMap = new BattleMap(BATTLE_MAP_SIZE);

    groups.Initialize(MAXIMUM_GROUP_COUNT);

    auto centerTile = battleMap->GetCenterTile();
    Integer3 offset = {-4, 12, -8};

    GroupFactory::Create(groups, {centerTile->GetNeighbor(offset)});
    GroupFactory::Create(groups, {centerTile->GetNeighbor(-offset)});

    battleTileModel = new BattleTileModel();
    battleTileModel->Initialize();

    battleInterface = new BattleInterface();
    battleInterface->Initialize();

    camera = RenderManager::GetCamera(Cameras::BATTLE);

    OnInitialized.Invoke();
}

void BattleScene::HandleEnable()
{
    DetermineTurnOrder();

    OnEnabled.Invoke();
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

    turnOrder.SortDescendantly();

    turn = turnOrder.GetStart();

    turn->Character->StartTurn();
}

void BattleScene::EndTurn()
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

bool BattleScene::IsCharactersTurn(Character* character) const
{
    return character == turn->Character;
}

Camera * BattleScene::GetCamera() const
{
    return camera;
}

Group * BattleScene::GetRandomGroup()
{
    auto index = utility::GetRandom(0, groups.GetSize() - 1);
    return groups.Get(index);
}

BattleScene* BattleScene::Get()
{
    if(instance == nullptr)
    {
        instance = new BattleScene();
    }
    
    return instance;
}