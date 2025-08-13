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

    for(auto &particle : flightParticles)
    {
        auto x = utility::GetRandom(-15.0f, 15.0f);

        auto y = utility::GetRandom(-15.0f, 15.0f);

        auto scale = utility::GetRandom(0.5f, 0.65f);

        auto tint = utility::GetRandom(0.3f, 1.0f);

        particle = Particle{{x, y}, scale, tint};
    }

    for(auto &particle : boomParticles)
    {
        auto x = utility::GetRandom(-10.0f, 10.0f);

        auto y = utility::GetRandom(-10.0f, 10.0f);

        auto scale = utility::GetRandom(0.8f, 1.1f);

        auto tint = utility::GetRandom(0.3f, 1.0f);

        particle = Particle{{x, y}, scale, tint};
    }

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

        engine::SoundManager::Get()->PlaySound("Explosion3");
    }

    if(phase == Phases::FLIGHT)
        strength = 0.4f;
    else
        strength = 2.0f - lifeTime;

    if(lifeTime > DURATION)
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

    shader->SetConstant(0.0f, "depth");

    if(phase == Phases::FLIGHT)
    {
        auto factor = lifeTime;

        auto position = startingTile->Position * (1.0f - factor) + targetTile->Position * factor;

        auto size = BattleMap::TILE_SIZE * 0.5f * (1.0f - factor) + BattleMap::TILE_SIZE * 1.2f * factor;

        for(auto &particle : flightParticles)
        {
            shader->SetConstant(strength, "opacity");

            shader->SetConstant(position + particle.Position, "hexPosition");

            shader->SetConstant(size * particle.Size, "hexSize");

            auto color = Color::ORANGE * particle.TintFactor + Color::YELLOW * (1.0f - particle.TintFactor);
            shader->SetConstant(color, "color");

            glDrawArrays(GL_TRIANGLES, 0, 18);
        }
    }
    else
    {
        shader->SetConstant(strength, "opacity");

        auto i = 0;

        auto &neighbours = targetTile->GetNearbyTiles(range);
        for(auto nearbyTile : neighbours)
        {
            auto &particle = *boomParticles.Get(i++);

            shader->SetConstant(nearbyTile->Position + particle.Position, "hexPosition");

            shader->SetConstant(BattleMap::TILE_SIZE * particle.Size, "hexSize");

            auto color = Color::ORANGE * particle.TintFactor + Color::YELLOW * (1.0f - particle.TintFactor);
            shader->SetConstant(color, "color");

            glDrawArrays(GL_TRIANGLES, 0, 18);
        }
    }

    shader->Unbind();
}