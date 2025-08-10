#include "FlumenCore/Observer.h"
#include "FlumenCore/Time.hpp"

#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/Camera.hpp"
#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Render/ShaderManager.hpp"
#include "FlumenEngine/Sound/SoundManager.h"

#include "FireballEffect.h"
#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/Battle/Render/BattleTileModel.h"
#include "FlumenBattle/Battle/BattleTile.h"
#include "FlumenBattle/Battle/BattleMap.h"

using namespace battle::render;

FireballEffect::FireballEffect(BattleTile *newTile, BattleTile *startTile, int newRange)
{
    isActive = true;

    lifeTime = 0.0f;

    strength = 1.0f;

    targetTile = newTile;

    startingTile = startTile;

    range = newRange;

    phase = Phases::FLIGHT;

    engine::SoundManager::Get()->PlaySound("FireballFlight");

    BattleTileModel::Get()->AddRenderJob({this, &FireballEffect::Render});
}

bool FireballEffect::Update()
{
    lifeTime += Time::GetDelta();

    if(lifeTime > 1.0f && phase == Phases::FLIGHT)
    {
        phase = Phases::EXPLOSION;

        engine::SoundManager::Get()->PlaySound("FireballBoom");
    }

    if(phase == Phases::FLIGHT)
        strength = 1.0f;
    else
        strength = 2.0f - lifeTime;

    if(lifeTime > 2.0f)
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

    shader->SetConstant(0.0f, "depth");

	shader->SetConstant(strength, "opacity");

    if(phase == Phases::FLIGHT)
    {
        auto factor = lifeTime;

        auto position = startingTile->Position * (1.0f - factor) + targetTile->Position * factor;

        shader->SetConstant(position, "hexPosition");

        auto size = BattleMap::TILE_SIZE * 0.3f * (1.0f - factor) + BattleMap::TILE_SIZE * 1.2f * factor;

        shader->SetConstant(size, "hexSize");

        shader->SetConstant(Color::ORANGE, "color");

        glDrawArrays(GL_TRIANGLES, 0, 18);
    }
    else
    {
        auto &neighbours = targetTile->GetNearbyTiles(range);
        for(auto nearbyTile : neighbours)
        {
            shader->SetConstant(nearbyTile->Position, "hexPosition");

            shader->SetConstant(BattleMap::TILE_SIZE, "hexSize");

            shader->SetConstant(Color::ORANGE, "color");

            glDrawArrays(GL_TRIANGLES, 0, 18);
        }
    }

    shader->Unbind();
}