#include "FlumenEngine/Interface/Sprite.hpp"
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
    decisionMenu->Configure(Size(420, 240), DrawOrder(3), Position2(0.0f, 0.0f), sprite, Opacity(0.85f));
}

void WorldInterface::Enable()
{
    decisionMenu->Enable();
}

void WorldInterface::Disable()
{
    decisionMenu->Disable();
}