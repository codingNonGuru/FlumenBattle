#include "FlumenCore/Observer.h"
#include "FlumenCore/Time.hpp"

#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/Camera.hpp"
#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Render/ShaderManager.hpp"

#include "FireballEffect.h"
#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/Battle/Render/BattleTileModel.h"
#include "FlumenBattle/Battle/BattleTile.h"
#include "FlumenBattle/Battle/BattleMap.h"

using namespace battle::render;

FireballEffect::FireballEffect(BattleTile *newTile, int newRange)
{
    isActive = true;

    lifeTime = 0.0f;

    strength = 1.0f;

    tile = newTile;

    range = newRange;

    BattleTileModel::Get()->AddRenderJob({this, &FireballEffect::Render});
}

bool FireballEffect::Update()
{
    lifeTime += Time::GetDelta();

    strength = 1.0f - lifeTime;

    if(lifeTime > 1.0f)
    {
        isActive = false;

        BattleTileModel::Get()->RemoveRenderJob({this, &FireballEffect::Render});

        return true;
    }

    return false;
}

void FireballEffect::Render()
{
    static auto camera = RenderManager::GetCamera(Cameras::BATTLE);

    static auto shader = ShaderManager::GetShader("Hex");

    shader->Bind();

	shader->SetConstant(camera->GetMatrix(), "viewMatrix");

	shader->SetConstant(BattleMap::TILE_SIZE, "hexSize");

	shader->SetConstant(strength, "opacity");

	shader->SetConstant(0.0f, "depth");

    auto &neighbours = tile->GetNearbyTiles(range);
    for(auto nearbyTile : neighbours)
    {
        shader->SetConstant(nearbyTile->Position, "hexPosition");

        shader->SetConstant(BattleMap::TILE_SIZE, "hexSize");

        shader->SetConstant(Color::ORANGE, "color");

        glDrawArrays(GL_TRIANGLES, 0, 18);
    }

    shader->Unbind();
}