#include "FlumenBattle/WorldState.h"
#include "FlumenBattle/WorldScene.h"
#include "FlumenBattle/WorldInterface.h"
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
}

void WorldState::HandleEnter()
{
    scene->Enable();

    interface->Enable();
}

void WorldState::HandleExit() 
{
    scene->Disable();

    interface->Disable();
}
