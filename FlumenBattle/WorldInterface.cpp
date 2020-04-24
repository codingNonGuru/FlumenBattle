#include "FlumenEngine/Interface/Sprite.hpp"
#include "FlumenEngine/Interface/Interface.hpp"
#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Render/ShaderManager.hpp"
#include "FlumenEngine/Core/Transform.hpp"

#include "FlumenBattle/WorldInterface.h"
#include "FlumenBattle/WorldDecisionMenu.h"

WorldInterface::WorldInterface()
{
    auto spriteShader = ShaderManager::GetShaderMap().Get("Sprite");

    auto sprite = new Sprite(nullptr, spriteShader);

    decisionMenu = new WorldDecisionMenu();
    Interface::AddElement("WorldDecisionMenu", decisionMenu);
    decisionMenu->Configure(Size(420, 240), DrawOrder(3), new Transform(Position2(0.0f, 0.0f)), sprite, Opacity(0.7f));
}

void WorldInterface::Enable()
{
    decisionMenu->Enable();
}

void WorldInterface::Disable()
{
    decisionMenu->Disable();
}