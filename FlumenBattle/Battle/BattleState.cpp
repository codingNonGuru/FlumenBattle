#include "FlumenBattle/Battle/BattleState.h"
#include "FlumenBattle/Battle/BattleScene.h"
#include "FlumenBattle/Battle/BattleController.h"
#include "FlumenBattle/Battle/BattleInterface.h"
#include "FlumenBattle/Battle/BattleTileModel.h"
#include "FlumenBattle/Battle/Sound/BattleMixer.h"

using namespace battle;

static BattleTileModel *tileModel = nullptr;

BattleState::BattleState()
{
    scene = BattleScene::Get();

    controller = BattleController::Get();

    tileModel = BattleTileModel::Get();

    interface = BattleInterface::Get();
}

bool isInitialized = false;

void BattleState::HandleEnter() 
{
    if(isInitialized == false)
    {
        scene->Initialize();
        
        controller->Initialize();

        interface->Initialize();

        sound::BattleMixer::Get()->Initialize();

        isInitialized = true;
    }

    scene->Enable();

    interface->Enable();

    tileModel->Initialize();
}

void BattleState::HandleExit() 
{
    interface->Disable();

    scene->Disable();
}