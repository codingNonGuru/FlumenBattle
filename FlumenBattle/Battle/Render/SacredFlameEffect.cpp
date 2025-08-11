#include "FlumenCore/Observer.h"
#include "FlumenCore/Time.hpp"

#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Render/ShaderManager.hpp"
#include "FlumenEngine/Sound/SoundManager.h"
#include "FlumenEngine/Render/DataBuffer.hpp"

#include "SacredFlameEffect.h"
#include "FlumenBattle/LineRenderer.h"
#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/Battle/Render/BattleTileModel.h"
#include "FlumenBattle/Battle/BattleTile.h"
#include "FlumenBattle/Battle/BattleMap.h"

using namespace battle::render;

#define BUFFER_COUNT 5

static const auto DURATION = 2.0f;

static render::LineRenderer *lineRenderer = nullptr;

SacredFlameEffect::SacredFlameEffect(BattleTile *newTile)
{
    if(lineRenderer == nullptr)
    {
        lineRenderer = new ::render::LineRenderer(particles.GetCapacity());
    }

    isActive = true;

    auto startPosition = newTile->Position - Position2{0.0f, BattleMap::TILE_SIZE * 15.0f};

    auto endPosition = newTile->Position;

    for(auto &particle : particles)
    {
        auto factor = utility::Clamp(utility::GetGaussianRandom(10.0f, 1.5f), 2.0f, 15.0f);

        auto angle = utility::Clamp(utility::GetGaussianRandom(0.0f, 0.06f), -0.35f, 0.35f) + HALF_PI;

        auto position = startPosition + Position2{cos(angle), sin(angle)} * BattleMap::TILE_SIZE * factor;

        auto scale = utility::GetRandom(7.0f, 10.0f);

        auto tint = utility::GetRandom(0.05f, 0.75f);

        auto rotation = angle + HALF_PI;

        particle = Particle{position, scale, tint, rotation, 5.0f};
    }

    lifeTime = 0.0f;

    strength = 1.0f;

    targetTile = newTile;

    engine::SoundManager::Get()->PlaySound("SacredFlame");

    BattleTileModel::Get()->AddRenderJob({this, &SacredFlameEffect::Render});
}

bool SacredFlameEffect::Update()
{
    lifeTime += Time::GetDelta();

    if(lifeTime > DURATION)
    {
        isActive = false;

        BattleTileModel::Get()->RemoveRenderJob({this, &SacredFlameEffect::Render});

        return true;
    }

    return false;
}

void SacredFlameEffect::TransferData()
{
    auto &positions = lineRenderer->GetPositions();
    positions.Reset();

    auto &rotations = lineRenderer->GetRotations();
    rotations.Reset();

    auto &lengths = lineRenderer->GetLengths();
    lengths.Reset();

    auto &thicknesses = lineRenderer->GetThicknesses();
    thicknesses.Reset();

    auto &colors = lineRenderer->GetColors();
    colors.Reset();

    for(auto &particle : particles)
    {
        *positions.Add() = particle.Position;

        *rotations.Add() = particle.Rotation;

        *lengths.Add() = particle.Size;

        *thicknesses.Add() = 3.0f;

        *colors.Add() = Color::RED * particle.TintFactor + Color::YELLOW * (1.0f - particle.TintFactor);
    }

    lineRenderer->TransferData(positions, rotations, lengths, thicknesses, colors);
}

void SacredFlameEffect::Render()
{
    TransferData();

    static auto camera = RenderManager::GetCamera(Cameras::BATTLE);

    auto factor = lifeTime / DURATION;

    auto opacityFactor = (1.0f - factor) * 0.8f;

    lineRenderer->Render(camera, opacityFactor);
}