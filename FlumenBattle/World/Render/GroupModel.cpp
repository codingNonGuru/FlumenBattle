#include "FlumenEngine/Render/ShaderManager.hpp"
#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Render/Camera.hpp"
#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/DataBuffer.hpp"
#include "FlumenEngine/Render/SquareRenderer.h"
#include "FlumenEngine/Interface/Sprite.hpp"

#include "GroupModel.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Tile/WorldMap.h"
#include "FlumenBattle/World/Tile/WorldTile.h"
#include "FlumenBattle/World/Tile/WorldBiome.h"
#include "FlumenBattle/World/Render/WorldTileModel.h"
#include "FlumenBattle/World/Group/GroupCore.h"
#include "FlumenBattle/World/Group/HumanMind.h"
#include "FlumenBattle/World/Group/GroupSpotting.h"
#include "FlumenBattle/Config.h"

using namespace world::render;

void GroupModel::Initialize()
{

}

void GroupModel::Render()
{
    static const auto camera = RenderManager::GetCamera(Cameras::WORLD);

    for(auto &group : WorldScene::Get()->GetGroups())
    {
        if(group.GetTile()->IsRevealed() == false)
            continue;

        auto position = group.GetVisualPosition();
        position += Position2(0.0f, -15.0f);

        engine::render::SquareRenderer::DrawSquare(camera, position, Scale2(18.0f, 30.0f), 0.0f, Color::WHITE, 1.0f, 0.4f);

        auto typeColor = [&]
        {
            if(group.GetClass() == group::GroupClasses::BANDIT)
            {
                return Color::RED;
            }
            else if(group.GetClass() == group::GroupClasses::PATROL)
            {
                return Color::BLUE;
            }
            else if(group.GetClass() == group::GroupClasses::MERCHANT)
            {
                return Color::ORANGE;
            }
            else if(group.GetClass() == group::GroupClasses::ADVENTURER)
            {
                return Color::GREEN;
            }
            else if(group.GetClass() == group::GroupClasses::RAIDER)
            {
                return Color::BLACK;
            }
            else if(group.GetClass() == group::GroupClasses::GARRISON)
            {
                return Color::YELLOW;
            }
            else if(group.GetClass() == group::GroupClasses::PLAYER)
            {
                return Color::MAGENTA;
            }
        } ();

        auto opacity = Opacity(1.0f);
        if(group.IsInEncounter() == false)
        {
            auto groupBuffer = WorldScene::Get()->GetNearbyGroups(group.GetTile(), 0);
            if(groupBuffer.Groups.GetSize() > 1)
            {
                opacity = Opacity(0.6f);
            }
            else
            {
                opacity = Opacity(0.2f);
            }
        }

        engine::render::SquareRenderer::DrawSquare(camera, position, Scale2(12.0f, 12.0f), 0.0f, typeColor, 1.0f, 0.45f);

        auto actionColor = [&]
        {
            if(group.IsDoingSomething() == false)
            {
                return Color::MAGENTA;
            }
            else if(group.IsDoing(group::GroupActions::TAKE_LONG_REST))
            {
                return Color::RED;
            }
            else if(group.IsDoing(group::GroupActions::TRAVEL))
            {
                return Color::BLUE;
            }
            else if(group.IsDoing(group::GroupActions::ENGAGE))
            {
                return Color::GREEN;
            }
        } ();

        engine::render::SquareRenderer::DrawSquare(camera, position + Position2(0.0f, -10.0f), Scale2(6.0f, 6.0f), 0.0f, actionColor, 1.0f, 0.45f);

        /*if(&group == playerGroup)
            continue;

        for(int i = 0; i < group.travelActionData->PlannedDestinationCount; ++i)
        {
            auto tile = group.travelActionData->Route[i];
            dotSprite->Draw(camera, {tile->Position, Scale2(0.75f, 0.75f), Opacity(0.6f), DrawOrder(-2)});
        }*/
    }
}

void GroupModel::RenderSightings()
{
    static const auto GROUP_VISUAL_SCALE = Scale2(0.25f);

    static const auto GROUP_VISUAL_OFFSET = Position2(0.0f, -15.0f);

    static const auto GROUP_SPOTTING_LIMIT = engine::ConfigManager::Get()->GetValue(game::ConfigValues::GROUP_SPOTTING_LIMIT).Integer;

    static DataBuffer *sightingPositionBuffer = nullptr;

    static DataBuffer *sightingOffsetBuffer = nullptr;

    static DataBuffer *sightingOpacityBuffer = nullptr;

    static DataBuffer *sightingFlipBuffer = nullptr;

    static auto positions = container::Array <Position2> (GROUP_SPOTTING_LIMIT);

    static auto offsets = container::Array <Position2> (GROUP_SPOTTING_LIMIT);

    static auto opacities = container::Array <float> (GROUP_SPOTTING_LIMIT);

    static auto flipStates = container::Array <int> (GROUP_SPOTTING_LIMIT);

    if(sightingPositionBuffer == nullptr)
    {
        sightingPositionBuffer = new DataBuffer(positions.GetMemoryCapacity(), positions.GetStart());

        sightingOffsetBuffer = new DataBuffer(offsets.GetMemoryCapacity(), offsets.GetStart());

        sightingOpacityBuffer = new DataBuffer(opacities.GetMemoryCapacity(), opacities.GetStart());

        sightingFlipBuffer = new DataBuffer(flipStates.GetMemoryCapacity(), flipStates.GetStart());
    }

    static auto massShader = ShaderManager::GetShader("ComplexMassSprite");

    static auto sightingSprite = new Sprite(massShader, "MerryFellow");

    
    static const auto camera = RenderManager::GetCamera(Cameras::WORLD);

    positions.Reset();

    offsets.Reset();

    opacities.Reset();

    flipStates.Reset();

    auto &sightings = group::HumanMind::Get()->GetGroupSightings();

    for(auto &sighting : sightings)
    {
        *positions.Add() = sighting.VisualPosition + GROUP_VISUAL_OFFSET;

        *offsets.Add() = Position2(0.0f);

        static auto &worldTime = WorldScene::Get()->GetTime();
        auto hoursElapsed = worldTime.TotalHourCount - sighting.TimeInHours;

        static const auto MAXIMUM_SPOTTING_LIFETIME = engine::ConfigManager::Get()->GetValue(game::ConfigValues::MAXIMUM_SPOTTING_LIFETIME).Integer;

        float opacityFactor = (float)hoursElapsed / (float)MAXIMUM_SPOTTING_LIFETIME;

        *opacities.Add() = 1.0f - opacityFactor;

        *flipStates.Add() = (int)sighting.IsFacingRightwards;
    }

    sightingPositionBuffer->UploadData(positions.GetStart(), positions.GetMemorySize());

    sightingOffsetBuffer->UploadData(offsets.GetStart(), offsets.GetMemorySize());

    sightingOpacityBuffer->UploadData(opacities.GetStart(), opacities.GetMemorySize());

    sightingFlipBuffer->UploadData(flipStates.GetStart(), flipStates.GetMemorySize());

    massShader->Bind();

    massShader->SetConstant(camera->GetMatrix(), "viewMatrix");

	massShader->SetConstant(0.0f, "depth");

    massShader->SetConstant(GROUP_VISUAL_SCALE.x, "spriteSize");

    massShader->SetConstant(Scale2(1.0f), "textureScale");

    massShader->SetConstant(0, "hasRotation");

    massShader->SetConstant(1, "hasOpacity");

    massShader->SetConstant(1, "hasFlip");

    sightingPositionBuffer->Bind(0);

    sightingOffsetBuffer->Bind(1);

    sightingOpacityBuffer->Bind(2);

    sightingFlipBuffer->Bind(3);

    sightingSprite->BindDefaultTextures();

    glDrawArrays(GL_TRIANGLES, 0, 6 * positions.GetSize());

    massShader->Unbind();

    static auto playerGroup = WorldScene::Get()->GetPlayerGroup();

    static auto playerSprite = new Sprite(ShaderManager::GetShader("Sprite"), "MerryFellowBlueGlow");

    playerSprite->Draw(
        camera, 
        {playerGroup->GetVisualPosition() + GROUP_VISUAL_OFFSET, GROUP_VISUAL_SCALE, Opacity(1.0f), DrawOrder(-1)}
        );

    static auto hoveredSightingSprite = new Sprite(ShaderManager::GetShader("Sprite"), "OrangeEdgedMerryFellow");

    static auto hoveredSightingSpriteFlipped = new Sprite(ShaderManager::GetShader("Sprite"), "OrangeEdgedMerryFellowFlip");

    auto hoveredSighting = group::HumanMind::Get()->GetHoveredSpotting();
    if(hoveredSighting != nullptr)
    {
        if(hoveredSighting->IsFacingRightwards == true)
        {
            hoveredSightingSpriteFlipped->Draw(
                camera, 
                {hoveredSighting->VisualPosition + GROUP_VISUAL_OFFSET, GROUP_VISUAL_SCALE, Opacity(1.0f), DrawOrder(-1)}
                );
        }
        else
        {
            hoveredSightingSprite->Draw(
                camera, 
                {hoveredSighting->VisualPosition + GROUP_VISUAL_OFFSET, GROUP_VISUAL_SCALE, Opacity(1.0f), DrawOrder(-1)}
                );
        }
    }
}