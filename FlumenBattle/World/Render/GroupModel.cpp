#include "FlumenEngine/Render/ShaderManager.hpp"
#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Render/Camera.hpp"
#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/DataBuffer.hpp"
#include "FlumenEngine/Render/SquareRenderer.h"
#include "FlumenEngine/Interface/Sprite.hpp"
#include "FlumenEngine/Render/TextureManager.hpp"
#include "FlumenEngine/Render/Texture.hpp"

#include "GroupModel.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Tile/WorldMap.h"
#include "FlumenBattle/World/Tile/WorldTile.h"
#include "FlumenBattle/World/Tile/WorldBiome.h"
#include "FlumenBattle/World/Render/WorldTileModel.h"
#include "FlumenBattle/World/Group/GroupCore.h"
#include "FlumenBattle/World/Group/HumanMind.h"
#include "FlumenBattle/World/Group/GroupSpotting.h"
#include "FlumenBattle/World/Group/GroupAllocator.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/World/Character/CharacterClass.h"
#include "FlumenBattle/World/Group/CharacterEssence.h"
#include "FlumenBattle/Config.h"

using namespace world::render;

void GroupModel::Initialize()
{

}

struct GroupRenderData
{
    container::Block <int, 16> ClassIndices;

    Position2 Position;

    int MemberCount;

    int Padding;
};

void GroupModel::Render()
{
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);

    static const auto GROUP_COUNT = group::GroupAllocator::Get()->GetMaximumGroupCount();

    static const auto MAXIMUM_CHARACTERS_PER_GROUP = engine::ConfigManager::Get()->GetValue(game::ConfigValues::MAXIMUM_CHARACTERS_PER_GROUP).Integer;

    static auto renderDatas = container::Array <GroupRenderData> (GROUP_COUNT);

    static DataBuffer *renderDataBuffer = new DataBuffer(renderDatas.GetMemoryCapacity(), renderDatas.GetStart());

    static const auto camera = RenderManager::GetCamera(Cameras::WORLD);

    renderDatas.Reset();

    int groupCount = 0;

    for(auto &group : WorldScene::Get()->GetGroups())
    {
        if(group.GetTile()->IsRevealed() == false)
            continue;

        auto position = group.GetVisualPosition();

        container::Block <int, 16> classIndices;

        for(auto i = 0; i < group.GetSize(); ++i)
        {
            auto characterClass = [&]
            {
                if(group.IsDeepGroup() == true)
                {
                    return group.GetCharacter(i)->GetClass()->Class;
                    
                }
                else
                {   
                    auto &characters = group.GetCharacterEssences();
                    return characters.Get(i)->characterClass;
                }
            } ();

            switch(characterClass)
            {
            case character::CharacterClasses::FIGHTER:
                *classIndices[i] = 0;
                break;
            case character::CharacterClasses::RANGER:
                *classIndices[i] = 1;
                break;
            case character::CharacterClasses::WIZARD:
                *classIndices[i] = 2;
                break;
            case character::CharacterClasses::CLERIC:
                *classIndices[i] = 3;
                break;
            }
        }

        *renderDatas.Add() = GroupRenderData{classIndices, position, group.GetSize()};

        groupCount++;
    }

    renderDataBuffer->UploadData(renderDatas.GetStart(), renderDatas.GetMemorySize());

    static const auto shader = ShaderManager::GetShader("Group");

    static const auto texture = ::render::TextureManager::GetTexture("WorldMapCharacters");

    shader->Bind();

    shader->SetConstant(camera->GetMatrix(), "viewMatrix");

	shader->SetConstant(0.4f, "depth");

    shader->SetConstant(MAXIMUM_CHARACTERS_PER_GROUP, "maximumGroupSize");

    renderDataBuffer->Bind(0);

    shader->BindTexture(texture, "diffuse");

    glDrawArrays(GL_TRIANGLES, 0, 6 * groupCount * MAXIMUM_CHARACTERS_PER_GROUP);

    shader->Unbind();

    glDisable(GL_MULTISAMPLE);
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