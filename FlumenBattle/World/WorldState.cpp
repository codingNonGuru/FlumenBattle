#include "FlumenCore/Delegate/Event.hpp"

#include "FlumenEngine/Core/Engine.hpp"

#include "FlumenBattle/World/WorldState.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Render/WorldTileModel.h"
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
        controller->Initialize();

        tileModel = render::WorldTileModel::Get();
        tileModel->Initialize();
    }

    void WorldState::HandleEnter()
    {
        scene->Enable();

        interface->Enable();

        controller->Enable();

        tileModel->Enable();

        Engine::OnInterfaceUpdateStarted += {interface, &WorldInterface::Update};
    }

    void WorldState::HandleExit() 
    {
        scene->Disable();

        interface->Disable();

        controller->Disable();

        Engine::OnInterfaceUpdateStarted -= {interface, &WorldInterface::Update};
    }
}