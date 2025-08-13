#include "FlumenCore/Observer.h"
#include "FlumenCore/Time.hpp"

#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/Camera.hpp"
#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Render/ShaderManager.hpp"
#include "FlumenEngine/Sound/SoundManager.h"

#include "BurningHandsEffect.h"
#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/Battle/Render/BattleTileModel.h"
#include "FlumenBattle/Battle/BattleTile.h"
#include "FlumenBattle/Battle/BattleMap.h"
#include "FlumenBattle/Battle/SpellCaster.h"
#include "FlumenBattle/Battle/Combatant.h"
#include "FlumenBattle/World/Character/Character.h"

using namespace battle::render;

BurningHandsEffect::BurningHandsEffect(Combatant &caster, BattleTile *targetTile)
{
    isActive = true;

    auto &tiles = SpellCaster::GetAffectedTiles(caster, *caster.GetCharacter()->GetSelectedSpell(), targetTile);

    particles.Clear();

    for(auto &tile : tiles)
    {
        for(int i = 0; i < 10; ++i)
        {
            auto x = utility::GetRandom(-25.0f, 25.0f);

            auto y = utility::GetRandom(-25.0f, 25.0f);

            auto scale = utility::GetRandom(0.4f, 0.5f);

            auto tint = utility::GetRandom(0.3f, 1.0f);

            auto opacity = utility::GetRandom(0.7f, 0.9f);

            auto particle = particles.Add();
            *particle = Particle{tile->Position + Position2{x, y}, scale, tint, opacity};
        }
    }

    lifeTime = 0.0f;

    strength = 1.0f;

    engine::SoundManager::Get()->PlaySound("FireballFlight");

    BattleTileModel::Get()->AddRenderJob({this, &BurningHandsEffect::Render});
}

bool BurningHandsEffect::Update()
{
    lifeTime += Time::GetDelta();

    strength = DURATION - lifeTime;

    strength /= DURATION;

    if(lifeTime > DURATION)
    {
        isActive = false;

        BattleTileModel::Get()->RemoveRenderJob({this, &BurningHandsEffect::Render});

        return true;
    }

    return false;
}

void BurningHandsEffect::Render()
{
    static auto camera = RenderManager::GetCamera(Cameras::BATTLE);

    static auto shader = ShaderManager::GetShader("Hex");

    shader->Bind();

	shader->SetConstant(camera->GetMatrix(), "viewMatrix");

    shader->SetConstant(0.0f, "depth");

    for(auto &particle : particles)
    {
        shader->SetConstant(strength * particle.Opacity, "opacity");

        shader->SetConstant(particle.Position, "hexPosition");

        shader->SetConstant(BattleMap::TILE_SIZE * particle.Size, "hexSize");

        auto color = Color::ORANGE * particle.TintFactor + Color::YELLOW * (1.0f - particle.TintFactor);
        shader->SetConstant(color, "color");

        glDrawArrays(GL_TRIANGLES, 0, 18);
    }

    shader->Unbind();
}