#include "FlumenCore/Observer.h"
#include "FlumenCore/Time.hpp"

#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/Camera.hpp"
#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Render/ShaderManager.hpp"
#include "FlumenEngine/Sound/SoundManager.h"
#include "FlumenEngine/Render/LineRenderer.h"

#include "FrostRayEffect.h"
#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/Battle/Render/BattleTileModel.h"
#include "FlumenBattle/Battle/BattleTile.h"

using namespace battle::render;

FrostRayEffect::FrostRayEffect(BattleTile *startingTile, BattleTile *targetTile)
{
    isActive = true;

    lifeTime = 0.0f;

    strength = 1.0f;

    this->startingTile = startingTile;

    this->targetTile = targetTile;

    engine::SoundManager::Get()->PlaySound("FrostRay");

    BattleTileModel::Get()->AddRenderJob({this, &FrostRayEffect::Render});
}

bool FrostRayEffect::Update()
{
    lifeTime += Time::GetDelta();

    strength = DURATION - lifeTime;

    strength /= DURATION;

    if(lifeTime > DURATION)
    {
        isActive = false;

        BattleTileModel::Get()->RemoveRenderJob({this, &FrostRayEffect::Render});

        return true;
    }

    return false;
}

void FrostRayEffect::Render()
{
    static auto camera = RenderManager::GetCamera(Cameras::BATTLE);

    auto position = startingTile->Position * 0.5f + targetTile->Position * 0.5f;

    auto direction = startingTile->Position - targetTile->Position;

    auto length = glm::length(direction);

    auto rotation = atan2(direction.y, direction.x);

    static auto color = Color::BLUE * 0.5f + Color::WHITE * 0.5f;

    engine::render::LineRenderer::RenderLine(camera, position, length, 12.0f, rotation, color, strength);
}