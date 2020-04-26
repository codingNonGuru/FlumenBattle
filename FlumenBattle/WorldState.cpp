#include "FlumenBattle/WorldState.h"
#include "FlumenBattle/WorldScene.h"
#include "FlumenBattle/WorldInterface.h"
#include "FlumenBattle/WorldController.h"
#include "FlumenBattle/Group.h"

WorldState::WorldState() 
{
    Initialize();
}

void WorldState::Initialize()
{
    scene = WorldScene::Get();
    scene->Initialize();

    interface = WorldInterface::Get();

    controller = WorldController::Get();
}

void WorldState::HandleEnter()
{
    scene->Enable();

    interface->Enable();

    controller->Enable();
}

void WorldState::HandleExit() 
{
    scene->Disable();

    interface->Disable();

    controller->Disable();
}
