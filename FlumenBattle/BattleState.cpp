#include "FlumenBattle/BattleState.h"
#include "FlumenBattle/BattleScene.h"
#include "FlumenBattle/BattleController.h"
#include "FlumenBattle/BattleInterface.h"
#include "FlumenBattle/BattleTileModel.h"

static BattleTileModel *tileModel = nullptr;

static BattleInterface *interface = nullptr;

BattleState::BattleState()
{
    scene = BattleScene::Get();

    controller = BattleController::Get();

    tileModel = BattleTileModel::Get();

    interface = new BattleInterface();
}

void BattleState::HandleEnter() 
{
    scene->Initialize();
    
    controller->Initialize();

    tileModel->Initialize();

    interface->Initialize();

    scene->Enable();
}

void BattleState::HandleExit() 
{
    interface->Disable();

    scene->Disable();
}