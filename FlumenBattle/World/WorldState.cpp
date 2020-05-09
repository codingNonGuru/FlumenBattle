#include "FlumenBattle/World/WorldState.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/WorldInterface.h"
#include "FlumenBattle/World/WorldController.h"
#include "FlumenBattle/World/Group/Group.h"

namespace world
{
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
}