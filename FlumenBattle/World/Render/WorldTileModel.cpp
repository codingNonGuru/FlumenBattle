#include "FlumenCore/Observer.h"
#include "FlumenCore/Time.hpp"

#include "FlumenEngine/Render/ShaderManager.hpp"
#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Render/Camera.hpp"
#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/MeshManager.hpp"
#include "FlumenEngine/Render/Mesh.hpp"
#include "FlumenEngine/Interface/Sprite.hpp"
#include "FlumenEngine/Core/Engine.hpp"
#include "FlumenEngine/Render/TextureManager.hpp"
#include "FlumenEngine/Render/DataBuffer.hpp"
#include "FlumenEngine/Core/Engine.hpp"
#include "FlumenEngine/Render/Screen.hpp"

#include "FlumenBattle/World/Render/WorldTileModel.h"
#include "FlumenBattle/World/Render/BorderModel.h"
#include "FlumenBattle/World/Render/FarmModel.h"
#include "FlumenBattle/World/Render/RendererAllocator.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/WorldController.h"
#include "FlumenBattle/World/WorldMap.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/Battle/Combatant.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/Group/GroupAction.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/SettlementTile.h"
#include "FlumenBattle/World/Settlement/TileImprovement.h"
#include "FlumenBattle/World/Settlement/Path.h"
#include "FlumenBattle/World/Polity/Polity.h"
#include "FlumenBattle/Utility/Pathfinder.h"
#include "FlumenBattle/World/Group/HumanMind.h"
#include "FlumenBattle/World/Group/GroupSpotting.h"
#include "FlumenBattle/World/Polity/HumanMind.h"
#include "FlumenBattle/Config.h"

#define WORLD_TILE_SIZE WorldMap::WORLD_TILE_SIZE

const Float CAMERA_SHIFT_DURATION = 0.5f;

static Camera* camera = nullptr;

static Float shadeTimer = 0.0f;

static Sprite *groupSprite = nullptr;

static Sprite *bootSprite = nullptr;

static Sprite *metalSprite = nullptr;

static Sprite *foodSprite = nullptr;

static Sprite *timberSprite = nullptr;

static Sprite *fiberSprite = nullptr;

static Sprite *farmSprite = nullptr;

static Sprite *dotSprite = nullptr;

static Sprite *xSprite = nullptr;

static Sprite *hammerSprite = nullptr;

using namespace world::render;

static world::WorldController * worldController = nullptr;

WorldTileModel::WorldTileModel()
{
    auto hexMesh = MeshManager::GetMeshes().Add("Hex"); 
	*hexMesh = Mesh::GenerateHex();

    shader = ShaderManager::GetShader("Hex");

    groupShader = ShaderManager::GetShader("Sprite");

    groupSprite = new Sprite(groupShader);

    bootSprite = new Sprite(groupShader, ::render::TextureManager::GetTexture("TravelBoot")); 

    metalSprite = new Sprite(groupShader, ::render::TextureManager::GetTexture("Metal"));

    foodSprite = new Sprite(groupShader, ::render::TextureManager::GetTexture("Radish"));

    timberSprite = new Sprite(groupShader, ::render::TextureManager::GetTexture("Timber"));

    fiberSprite = new Sprite(groupShader, ::render::TextureManager::GetTexture("Wool"));

    farmSprite = new Sprite(groupShader, ::render::TextureManager::GetTexture("FarmImprovement"));

    dotSprite = new Sprite(groupShader, ::render::TextureManager::GetTexture("Dot")); 

    xSprite = new Sprite(groupShader, ::render::TextureManager::GetTexture("X")); 

    hammerSprite = new Sprite(groupShader, ::render::TextureManager::GetTexture("WorkHammer")); 

    worldScene = WorldScene::Get();

    worldController = WorldController::Get();

    CreateCamera();

    BorderModel::Get()->Initialize();
}

void WorldTileModel::Initialize()
{   
    const auto size = worldScene->GetWorldMap()->GetSize();

    RendererAllocator::Get()->Allocate(size);

    FarmModel::Get()->Initialize();
}

void WorldTileModel::Enable()
{
    auto backgroundColor = Color(0.0f, 0.0f, 0.0f, 1.0f);
	RenderManager::SetBackgroundColor(backgroundColor);
}

void WorldTileModel::CreateCamera()
{
    auto screen = Engine::GetScreen();

	camera = new Camera(screen);
	RenderManager::AddCamera(Cameras::WORLD, camera);

    auto centerTile = worldScene->GetWorldMap()->GetCenterTile();
    camera->SetTarget(Position3(centerTile->Position, 0.0f));
    camera->Zoom(2.0f);
}

void WorldTileModel::HandleCharacterSelected()
{
    //auto combatant = battleController->GetSelectedCombatant();
    //camera->SetTarget(Position3(combatant->GetPosition(), 0.0f), CAMERA_SHIFT_DURATION);
}

Color WorldTileModel::GetGlobalLightColor()
{
    auto &time = worldScene->GetTime();
    Color color;

    Color nightColor = Color::BLUE * 0.4f + Color::BLACK * 0.6f;
    nightColor.a_ = 1.0f;

    Color eveningColor = Color::BLUE * 0.4f + Color::BLACK * 0.6f;
    eveningColor.a_ = 0.7f;

    Color noonColor = Color::YELLOW;
    noonColor.a_ = 0.05f;

    Color dawnColor = Color::ORANGE;
    dawnColor.a_ = 0.2f;

    if(time.HourCount >= 23 || time.HourCount < 5)
    {
        color = nightColor;
    }
    else if(time.HourCount >= 5 && time.HourCount < 6)
    {
        auto factor = (float)time.MinuteCount / 60.0f;
        color = nightColor * (1.0f - factor) + dawnColor * factor;
    }
    else if(time.HourCount >= 6 && time.HourCount < 7)
    {
        auto factor = (float)time.MinuteCount / 60.0f;
        color = dawnColor * (1.0f - factor) + noonColor * factor;
    }
    else if(time.HourCount >= 7 && time.HourCount < 19)
    {
        color = noonColor;
    }
    else if(time.HourCount >= 19 && time.HourCount < 20)
    {
        auto factor = (float)time.MinuteCount / 60.0f;
        color = noonColor * (1.0f - factor) + dawnColor * factor;
    }
    else if(time.HourCount >= 20 && time.HourCount < 21)
    {
        auto factor = (float)time.MinuteCount / 60.0f;
        color = dawnColor * (1.0f - factor) + eveningColor * factor;
    }
    else if(time.HourCount >= 21 && time.HourCount < 22)
    {
        color = eveningColor;
    }
    else if(time.HourCount >= 22 && time.HourCount < 23)
    {
        auto factor = (float)time.MinuteCount / 60.0f;
        color = eveningColor * (1.0f - factor) + nightColor * factor;
    }

    return color;
}

void WorldTileModel::RenderTiles()
{
    shader->Bind();

	shader->SetConstant(camera->GetMatrix(), "viewMatrix");

	shader->SetConstant(WORLD_TILE_SIZE, "hexSize");

	shader->SetConstant(1.0f, "opacity");

	shader->SetConstant(0.0f, "depth");

    auto map = worldScene->GetWorldMap();
    for(auto tile = map->GetTiles().GetStart(); tile != map->GetTiles().GetEnd(); ++tile)
    {
        shader->SetConstant(tile->Position, "hexPosition");

        shader->SetConstant(tile->GetShade(), "color");

        glDrawArrays(GL_TRIANGLES, 0, 18);
    }

    shader->Unbind();
}

void WorldTileModel::RenderPaths()
{
    auto squareShader = ShaderManager::GetShader("Square");
    squareShader->Bind();

    squareShader->SetConstant(camera->GetMatrix(), "viewMatrix");

	squareShader->SetConstant(1.0f, "opacity");

	squareShader->SetConstant(0.0f, "depth");

    auto pathSegments = worldScene->pathSegments;
    for(auto &segment : *pathSegments)
    {
        auto tile = segment.To;
        auto nextTile = segment.From;

        auto position = (tile->Position + nextTile->Position) / 2.0f;
        squareShader->SetConstant(position, "hexPosition");

        Scale2 scale = Scale2(WORLD_TILE_SIZE * 1.732f, 5.0f);
        squareShader->SetConstant(scale, "hexSize");

        auto color = [&segment] () {
            return segment.Type == settlement::RoadTypes::UNTRODDEN ?
            Color(0.9f, 0.7f, 0.5f, 1.0f) * 0.6f :
            Color(0.7f, 0.7f, 0.7f, 1.0f) * 0.6f;
        } ();
        squareShader->SetConstant(color, "color");

        auto orientation = tile->Position - nextTile->Position;
        auto rotation = atan2(orientation.y, orientation.x);
        squareShader->SetConstant(rotation, "rotation");

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    squareShader->Unbind();
}

void WorldTileModel::RenderPoliticalOverlay()
{
    shader->Bind();

    shader->SetConstant(camera->GetMatrix(), "viewMatrix");

	shader->SetConstant(0.4f, "opacity");

	shader->SetConstant(0.0f, "depth");

    auto map = worldScene->GetWorldMap();
    for(auto tile = map->GetTiles().GetStart(); tile != map->GetTiles().GetEnd(); ++tile)
    {
        if(tile->IsOwned() == false)
            continue;
        
        shader->SetConstant(tile->Position, "hexPosition");

        shader->SetConstant(WORLD_TILE_SIZE, "hexSize");

        shader->SetConstant(tile->GetOwner()->GetRulerBanner(), "color");

        glDrawArrays(GL_TRIANGLES, 0, 18);
    }

    shader->Unbind();
}

void WorldTileModel::RenderInterestMap()
{
    shader->Bind();

    shader->SetConstant(camera->GetMatrix(), "viewMatrix");

	shader->SetConstant(0.4f, "opacity");

	shader->SetConstant(0.0f, "depth");

    auto map = worldScene->GetWorldMap();
    auto polity = worldScene->polities->Get(0);
    auto &interestTiles = polity->GetInterestMap().GetTiles();
    for(auto tile = interestTiles.GetStart(); tile != interestTiles.GetEnd(); ++tile)
    {
        if(tile->GetOwner() == nullptr)
            continue;
        
        shader->SetConstant(map->GetTile(tile->Coordinates)->Position, "hexPosition");

        shader->SetConstant(WORLD_TILE_SIZE, "hexSize");

        shader->SetConstant(tile->GetOwner()->GetBanner(), "color");

        glDrawArrays(GL_TRIANGLES, 0, 18);
    }

    shader->Unbind();
}

void WorldTileModel::RenderSettlements()
{
    shader->Bind();

    shader->SetConstant(camera->GetMatrix(), "viewMatrix");

	shader->SetConstant(0.0f, "depth");

    shader->SetConstant(1.0f, "opacity");

    auto settlements = worldScene->GetSettlements();
    for(auto& settlement : settlements)
    {
        auto tile = settlement.GetLocation();

        shader->SetConstant(tile->Position, "hexPosition");

        auto size = [&settlement]
        {
            auto factor = pow((float)settlement.GetPopulation() + 1.0f, 0.5f);
            return factor * 0.15f;
        } ();
        shader->SetConstant(WORLD_TILE_SIZE * size, "hexSize");

        shader->SetConstant(settlement.GetRulerBanner(), "color");
        //shader->SetConstant(settlement.GetBanner(), "color");

        glDrawArrays(GL_TRIANGLES, 0, 18);
    }

    shader->Unbind();
}

DataBuffer *positionBuffer = nullptr;

DataBuffer *colorBuffer = nullptr;

DataBuffer *heatBuffer = nullptr;

void WorldTileModel::RenderGlobalLight()
{    
    auto map = worldScene->GetWorldMap();

    auto hexShader = ShaderManager::GetShader("UniformHex");

    hexShader->Bind();

    hexShader->SetConstant(camera->GetMatrix(), "viewMatrix");

	hexShader->SetConstant(0.0f, "depth");

    hexShader->SetConstant(0.8f, "opacity");

    auto lightColor = GetGlobalLightColor();
    hexShader->SetConstant(lightColor, "color");

    hexShader->SetConstant(WORLD_TILE_SIZE, "hexSize");

    positionBuffer->Bind(0);

    glDrawArrays(GL_TRIANGLES, 0, 18 * map->GetTileCount());

    hexShader->Unbind();
}

void WorldTileModel::RenderTilesAdvanced()
{
    static const auto map = worldScene->GetWorldMap();

    auto newHexShader = ShaderManager::GetShader("BetterHex");

    newHexShader->Bind();

    newHexShader->SetConstant(camera->GetMatrix(), "viewMatrix");

	newHexShader->SetConstant(0.0f, "depth");

    newHexShader->SetConstant(1.0f, "opacity");

    newHexShader->SetConstant(WORLD_TILE_SIZE, "hexSize");

    positionBuffer->Bind(0);

    colorBuffer->Bind(1);

    glDrawArrays(GL_TRIANGLES, 0, 18 * map->GetTileCount());

    newHexShader->Unbind();
}

void WorldTileModel::RenderSnow()
{
    static const DataBuffer *iceTileIndexBuffer = nullptr;
    if(iceTileIndexBuffer == nullptr)
    {
        static const auto map = worldScene->GetWorldMap();

        static auto indices = container::Array <int> (map->GetTileCount());

        for(auto tile = map->GetTiles().GetStart(); tile != map->GetTiles().GetEnd(); ++tile)
        {
            *indices.Add() = utility::RollD100Dice() < 50 ? 0 : 1;
        }

        iceTileIndexBuffer = new DataBuffer(indices.GetMemorySize(), indices.GetStart());
    }

    static const auto map = worldScene->GetWorldMap();

    static const auto snowShader = ShaderManager::GetShader("Snow");

    static const auto iceTileSprite = new Sprite(snowShader, "IceTile1");

    static const auto iceTile2Sprite = new Sprite(snowShader, "IceTile2");

    static const auto iceTileShrunkSprite = new Sprite(snowShader, "IceTileShrunk1");

    static const auto iceTileShrunk2Sprite = new Sprite(snowShader, "IceTileShrunk2");

    snowShader->Bind();

    snowShader->SetConstant(camera->GetMatrix(), "viewMatrix");

	snowShader->SetConstant(0.0f, "depth");

    snowShader->SetConstant(WORLD_TILE_SIZE, "hexSize");

    static auto &worldTime = worldScene->GetTime();

    auto seasonalFactor = WorldTile::GetSeasonalTemperatureSwing() / 100.0f;

    auto diurnalFactor = WorldTile::GetDiurnalTemperatureSwing() / 100.0f;

    snowShader->SetConstant(seasonalFactor + diurnalFactor, "weatherFactor");

    positionBuffer->Bind(0);

    heatBuffer->Bind(1);

    iceTileIndexBuffer->Bind(2);

    iceTileSprite->BindTexture("diffuse");

    iceTile2Sprite->BindTexture("diffuse2");

    iceTileShrunkSprite->BindTexture("diffuseShrunk");

    iceTileShrunk2Sprite->BindTexture("diffuseShrunk2");

    glDrawArrays(GL_TRIANGLES, 0, 18 * map->GetTileCount());

    snowShader->Unbind();
}

void WorldTileModel::RenderPlayerPath()
{
    if(WorldController::Get()->IsTravelPlanActive() == true)
    {
        auto pathData = WorldController::Get()->GetPlannedPath();

        for(auto &tile : pathData.Tiles)
        {
            if(&tile == pathData.Tiles.GetLast() - 1)
                break;

            auto nextTile = &tile + 1;

            float factor = 0.0f;
            for(int i = 0; i <= 5; i++, factor += 0.2f)
            {
                auto position = tile->Position * factor + (*nextTile)->Position * (1.0f - factor);
                dotSprite->Draw(camera, {position, Scale2(0.3f, 0.3f), Opacity(0.6f), DrawOrder(-2)});
            }
        }
    }

    const auto fullPathData = group::HumanMind::Get()->GetFullPathData();
    for(auto &tile : fullPathData.Tiles)
    {
        dotSprite->Draw(camera, {tile->Position, Scale2(0.75f, 0.75f), Opacity(0.6f), DrawOrder(-2)});
    }

    auto finalDestination = group::HumanMind::Get()->GetFinalDestination();
    if(finalDestination != nullptr)
    {
        auto position = finalDestination->Position;
        xSprite->Draw(camera, {position, Scale2(1.0f, 1.0f), Opacity(1.0f), DrawOrder(-2)});
    }
}

void WorldTileModel::RenderGroupSightings()
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

void WorldTileModel::RenderImprovements()
{
    if(WorldController::Get()->ShouldDisplayImprovements() == false)
        return;

    static const auto playerGroup = WorldScene::Get()->GetPlayerGroup();

    const auto playerTile = playerGroup->GetTile();
    const auto &nearbyTiles = playerTile->GetNearbyTiles(5);

    for(auto &tile : nearbyTiles.Tiles)
    {
        if(tile->IsOwned() == false)
            continue;

        const auto owner = tile->GetOwner();        
        if(owner->IsTileImproved(tile) == false)
            continue;

        farmSprite->Draw(camera, {tile->Position + Position2(0.0f, -15.0f), Scale2(0.55f, 0.55f), Opacity(1.0f), DrawOrder(-2)});
    }
}

void WorldTileModel::RenderBorderExpansionMap()
{
    if(WorldController::Get()->IsBorderExpandActive() == false)
        return;

    static const auto playerGroup = WorldScene::Get()->GetPlayerGroup();
    const auto playerSettlement = WorldScene::Get()->GetPlayerSettlement();

    static const auto BORDER_EXPANSION_MAX_DISTANCE = engine::ConfigManager::Get()->GetValue(game::ConfigValues::BORDER_EXPANSION_MAX_DISTANCE).Integer;

    const auto playerTile = playerGroup->GetTile();
    const auto &nearbyTiles = playerTile->GetNearbyTiles(BORDER_EXPANSION_MAX_DISTANCE);

    shader->Bind();

    shader->SetConstant(camera->GetMatrix(), "viewMatrix");

	shader->SetConstant(0.0f, "depth");

    shader->SetConstant(0.7f, "opacity");

    shader->SetConstant(WORLD_TILE_SIZE, "hexSize");

    for(auto &tile : nearbyTiles.Tiles)
    {
        if(tile->IsOwned() == true)
            continue;

        auto color = [&]
        {
            bool hasAtLeastOneNeighbour = false;

            auto immediateNeighbours = tile->GetNearbyTiles();
            for(auto &neighbour : immediateNeighbours)
            {
                if(neighbour->GetOwner() == playerSettlement)
                {
                    hasAtLeastOneNeighbour = true;
                    break;
                }
            }

            if(hasAtLeastOneNeighbour == true)
            {
                bool canAffordToExpand = playerSettlement->CanAffordToExpandHere(tile);
                bool hasExplored = playerSettlement->HasExplored(tile);

                if(canAffordToExpand == true && hasExplored == true)
                    return Color::GREEN;
                else if(canAffordToExpand == true || hasExplored == true)
                    return Color::YELLOW;
                else
                    return Color::ORANGE;
            }
            else
                return Color::RED;
        } ();

        shader->SetConstant(tile->Position, "hexPosition");

        shader->SetConstant(color, "color");

        glDrawArrays(GL_TRIANGLES, 0, 18);
    }

    shader->Unbind();

    auto &finishedExplorations = playerSettlement->GetFinishedExplorations();

    for(auto &exploration : finishedExplorations)
    {
        static const auto mapSprite = new Sprite(groupShader, ::render::TextureManager::GetTexture("MapStroked"));

        mapSprite->Draw(camera, {exploration.Tile->Position, Scale2(0.7f), Opacity(1.0f), DrawOrder(-2)});
    }

    auto currentExploration = playerSettlement->GetCurrentlyExploredTile();
    if(currentExploration != nullptr)
    {
        static const auto backpackSprite = new Sprite(groupShader, ::render::TextureManager::GetTexture("BackpackStroked"));

        backpackSprite->Draw(camera, {currentExploration->Position, Scale2(0.7f), Opacity(1.0f), DrawOrder(-2)});
    }
}

void WorldTileModel::RenderSettleModeMap()
{
    if(WorldController::Get()->IsSettleModeActive() == false)
        return;

    static const auto playerGroup = WorldScene::Get()->GetPlayerGroup();
    const auto playerSettlement = WorldScene::Get()->GetPlayerSettlement();

    const auto playerTile = playerGroup->GetTile();

    shader->Bind();

    shader->SetConstant(camera->GetMatrix(), "viewMatrix");

	shader->SetConstant(0.0f, "depth");

    shader->SetConstant(0.7f, "opacity");

    shader->SetConstant(WORLD_TILE_SIZE, "hexSize");

    for(int i = MINIMUM_COLONIZATION_RANGE; i <= MAXIMUM_COLONIZATION_RANGE; ++i)
    {
        auto tileRing = playerTile->GetTileRing(i);
        for(auto &tile : tileRing)
        {
            if(tile->IsBorderingOwnedTile())
                continue;

            if(tile->HasRelief(WorldReliefs::SEA) == true)
                continue;

            shader->SetConstant(tile->Position, "hexPosition");

            auto color = [&]
            {
                if(playerSettlement->HasAnySettlers() == true)
                {
                    return Color::GREEN;
                }   
                else
                {
                    return Color::YELLOW;
                }
            } ();

            shader->SetConstant(color, "color");

            glDrawArrays(GL_TRIANGLES, 0, 18);
        }
    }

    auto settleTarget = polity::HumanMind::Get()->GetSettleTarget(playerSettlement);
    if(settleTarget == nullptr)
        return;

    static const auto alphaSpriteShader = ShaderManager::GetShader("AlphaSprite");

    static const auto bannerFrameCore = new Sprite(alphaSpriteShader, ::render::TextureManager::GetTexture("BannerCore"));

    const auto banner = playerSettlement->GetBanner();
    bannerFrameCore->SetColor(&banner);

    bannerFrameCore->Draw(camera, {settleTarget->Position + Position2(0.0f, -WORLD_TILE_SIZE * 0.5f), Scale2(1.0f), Opacity(1.0f), DrawOrder(-2)});

    static const auto bannerFrameSprite = new Sprite(groupShader, ::render::TextureManager::GetTexture("BannerFrame"));

    bannerFrameSprite->Draw(camera, {settleTarget->Position + Position2(0.0f, -WORLD_TILE_SIZE * 0.5f), Scale2(1.0f), Opacity(1.0f), DrawOrder(-2)});
}

void WorldTileModel::RenderExploreMap()
{
    if(WorldController::Get()->IsExploreModeActive() == false)
        return;

    static const auto playerGroup = WorldScene::Get()->GetPlayerGroup();
    const auto playerSettlement = WorldScene::Get()->GetPlayerSettlement();

    const auto playerTile = playerGroup->GetTile();
    const auto &nearbyTiles = playerTile->GetNearbyTiles(MAXIMUM_COLONIZATION_RANGE);

    shader->Bind();

    shader->SetConstant(camera->GetMatrix(), "viewMatrix");

	shader->SetConstant(0.0f, "depth");

    shader->SetConstant(0.7f, "opacity");

    shader->SetConstant(WORLD_TILE_SIZE, "hexSize");

    for(auto &tile : nearbyTiles.Tiles)
    {
        if(playerSettlement->CanExploreHere(tile) == false)
            continue;

        if(playerSettlement->IsExploring(tile) == true)
            continue;

        if(playerSettlement->HasExplored(tile) == true)
            continue;

        shader->SetConstant(tile->Position, "hexPosition");

        auto color = [&]
        {
            /*if(playerSettlement->HasAnySettlers() == true)
            {*/
                return Color::GREEN;
            /*}   
            else
            {
                return Color::YELLOW;
            }*/
        } ();

        shader->SetConstant(color, "color");

        glDrawArrays(GL_TRIANGLES, 0, 18);
    }

    shader->Unbind();

    auto &finishedExplorations = playerSettlement->GetFinishedExplorations();

    for(auto &exploration : finishedExplorations)
    {
        static const auto mapSprite = new Sprite(groupShader, ::render::TextureManager::GetTexture("MapStroked"));

        mapSprite->Draw(camera, {exploration.Tile->Position, Scale2(0.7f), Opacity(1.0f), DrawOrder(-2)});
    }

    auto currentExploration = playerSettlement->GetCurrentlyExploredTile();
    if(currentExploration != nullptr)
    {
        static const auto backpackSprite = new Sprite(groupShader, ::render::TextureManager::GetTexture("BackpackStroked"));

        backpackSprite->Draw(camera, {currentExploration->Position, Scale2(0.7f), Opacity(1.0f), DrawOrder(-2)});
    }
}

void WorldTileModel::RenderTileDevelopMap()
{
    if(WorldController::Get()->IsTileDevelopModeActive() == false)
        return;

    static const auto playerGroup = WorldScene::Get()->GetPlayerGroup();
    const auto playerSettlement = WorldScene::Get()->GetPlayerSettlement();

    const auto playerTile = playerGroup->GetTile();

    shader->Bind();

    shader->SetConstant(camera->GetMatrix(), "viewMatrix");

	shader->SetConstant(0.0f, "depth");

    shader->SetConstant(0.7f, "opacity");

    shader->SetConstant(WORLD_TILE_SIZE, "hexSize");

    auto improvement = polity::HumanMind::Get()->GetProposedImprovement();

    for(auto &tile : playerSettlement->GetTiles())
    {
        if(playerSettlement->CanImproveHere(tile.Tile, improvement) == false)
            continue;

        if(playerSettlement->IsImprovingTile(tile.Tile, improvement) == true)
            continue;

        shader->SetConstant(tile.Tile->Position, "hexPosition");

        auto color = [&]
        {
            /*if(playerSettlement->HasAnySettlers() == true)
            {*/
                return Color::GREEN;
            /*}   
            else
            {
                return Color::YELLOW;
            }*/
        } ();

        shader->SetConstant(color, "color");

        glDrawArrays(GL_TRIANGLES, 0, 18);
    }

    shader->Unbind();

    static const auto improvementSprite = new Sprite(groupShader, ::render::TextureManager::GetTexture("FarmImprovement"));

    for(auto &tile : playerSettlement->GetTiles())
    {
        auto improvement = tile.GetImprovementType();
        if(improvement == nullptr)
            continue;

        improvementSprite->SetTexture(improvement->TextureName);

        improvementSprite->Draw(camera, {tile.Tile->Position, Scale2(1.0f), Opacity(1.0f), DrawOrder(-2)});
    }
}

void WorldTileModel::Render() 
{
    static auto index = 0;
    static auto durationSum = 0;

    auto startClock = high_resolution_clock::now();

    auto map = worldScene->GetWorldMap();

    if(positionBuffer == nullptr)
    {
        static auto positions = container::Array <Position2> (map->GetTileCount());

        static auto colors = container::Array <Color> (map->GetTileCount());

        static auto temperatures = container::Array <Float> (map->GetTileCount());

        for(auto tile = map->GetTiles().GetStart(); tile != map->GetTiles().GetEnd(); ++tile)
        {
            *positions.Add() = tile->Position;

            //*colors.Add() = Color((float)tile->Elevation / 100.0f);
            *colors.Add() = tile->GetShade();

            *temperatures.Add() = tile->Type == WorldTiles::LAND ? (float)tile->Heat / (float)WorldTile::MAXIMUM_TILE_HEAT : 1.0f;
        }

        positionBuffer = new DataBuffer(positions.GetMemorySize(), positions.GetStart());

        colorBuffer = new DataBuffer(colors.GetMemorySize(), colors.GetStart());

        heatBuffer = new DataBuffer(temperatures.GetMemorySize(), temperatures.GetStart());
    }

    RenderTilesAdvanced();

    FarmModel::Get()->Render();

    RenderSnow();

    //RenderPaths();

    BorderModel::Get()->Render();

    //RenderPoliticalOverlay();

    //RenderInterestMap();

    RenderSettlements();

    if(WorldController::Get()->ShouldDisplayMetal() == true)
    {
        auto map = worldScene->GetWorldMap();
        for(auto tile = map->GetTiles().GetStart(); tile != map->GetTiles().GetEnd(); ++tile)
        {
            if(tile->GetResource(settlement::ResourceTypes::METAL) == 0)
                continue;

            metalSprite->Draw(camera, {tile->Position, Scale2(1.0f, 1.0f) * 1.5f, Opacity(1.0f), DrawOrder(-2)});
        }
    }

    static const auto playerGroup = WorldScene::Get()->GetPlayerGroup();

    if(WorldController::Get()->ShouldDisplayNearbyFood() == true || WorldController::Get()->ShouldDisplayNearbyTimber() == true || WorldController::Get()->ShouldDisplayNearbyFiber() == true)
    {
        auto playerTile = playerGroup->GetTile();
        auto &nearbyTiles = playerTile->GetNearbyTiles(5);

        for(auto &tile : nearbyTiles.Tiles)
        {
            auto resourceAmount = tile->GetResource(
                [&] 
                {
                    if(WorldController::Get()->ShouldDisplayNearbyFood())
                        return settlement::ResourceTypes::FOOD; 
                    if(WorldController::Get()->ShouldDisplayNearbyTimber())
                        return settlement::ResourceTypes::TIMBER; 
                    if(WorldController::Get()->ShouldDisplayNearbyFiber())
                        return settlement::ResourceTypes::FIBER; 
                } ()
            );

            if(resourceAmount > 0)
            {
                for(auto i = 0; i < resourceAmount; ++i)
                {   
                    auto sprite = [&] 
                    {
                        if(WorldController::Get()->ShouldDisplayNearbyFood())
                            return foodSprite; 
                        if(WorldController::Get()->ShouldDisplayNearbyTimber())
                            return timberSprite; 
                        if(WorldController::Get()->ShouldDisplayNearbyFiber())
                            return fiberSprite; 
                    } (); 

                    auto positionOffset = ((float)i + 0.5f - (float)resourceAmount * 0.5f) * Position2(-10.0f, 0.0f);
                    sprite->Draw(camera, {tile->Position + positionOffset, Scale2(0.75f, 0.75f), Opacity(1.0f), DrawOrder(-2)});
                }
            }
        }
    }

    //RenderGroupSightings();

    RenderImprovements();

    RenderBorderExpansionMap();

    RenderSettleModeMap();

    RenderExploreMap();

    RenderTileDevelopMap();

    for(auto &group : *worldScene->groups)
    {
        auto position = group.GetVisualPosition();
        position += Position2(0, -15);

        groupSprite->SetColor(&Color::WHITE);
        groupSprite->Draw(
            camera, 
            {position, Scale2(18, 30), Opacity(1.0f), DrawOrder(-1)}
            );

        if(group.GetClass() == group::GroupClasses::BANDIT)
        {
            groupSprite->SetColor(&Color::RED);
        }
        else if(group.GetClass() == group::GroupClasses::PATROL)
        {
            groupSprite->SetColor(&Color::BLUE);
        }
        else if(group.GetClass() == group::GroupClasses::MERCHANT)
        {
            groupSprite->SetColor(&Color::ORANGE);
        }
        else if(group.GetClass() == group::GroupClasses::ADVENTURER)
        {
            groupSprite->SetColor(&Color::GREEN);
        }
        else if(group.GetClass() == group::GroupClasses::RAIDER)
        {
            groupSprite->SetColor(&Color::BLACK);
        }
        else if(group.GetClass() == group::GroupClasses::GARRISON)
        {
            groupSprite->SetColor(&Color::YELLOW);
        }
        else if(group.GetClass() == group::GroupClasses::PLAYER)
        {
            groupSprite->SetColor(&Color::MAGENTA);
        }

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

        groupSprite->Draw(
            camera, 
            {position, Scale2(12, 12), opacity, DrawOrder(0)}
            );

        if(group.IsDoingSomething() == false)
        {
            groupSprite->SetColor(&Color::MAGENTA);
        }
        else if(group.IsDoing(group::GroupActions::TAKE_LONG_REST))
        {
            groupSprite->SetColor(&Color::RED);
        }
        else if(group.IsDoing(group::GroupActions::TRAVEL))
        {
            groupSprite->SetColor(&Color::BLUE);
        }
        else if(group.IsDoing(group::GroupActions::ENGAGE))
        {
            groupSprite->SetColor(&Color::GREEN);
        }

        groupSprite->Draw(
            camera, 
            {position + Position2(0, -10), Scale2(6, 6), Opacity(1.0f), DrawOrder(0)}
            );

        /*if(&group == playerGroup)
            continue;

        for(int i = 0; i < group.travelActionData.PlannedDestinationCount; ++i)
        {
            auto tile = group.travelActionData.Route[i];
            dotSprite->Draw(camera, {tile->Position, Scale2(0.75f, 0.75f), Opacity(0.6f), DrawOrder(-2)});
        }*/
    }

    //RenderGlobalLight();

    RenderPlayerPath();

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - startClock);

    durationSum += duration.count();
    index++;

    if(index == 60)
    {
        //std::cout << "render average duration " << durationSum / 60 << "\n";

        index = 0;
        durationSum = 0;
    }
}

Rectangle WorldTileModel::GetFrustum()
{
    static auto worldMap = worldScene->GetWorldMap();

    const auto targetPosition =  camera->GetTarget();
    const auto targetTile = worldMap->GetTile(Float2{targetPosition.x, targetPosition.y});

    const auto coordinates = targetTile->SquareCoordinates;

    static const auto CAMERA_TO_WORLD_FACTOR = 9.0f;

    const auto zoomFactor = (0.0f + camera->GetZoomFactor()) * CAMERA_TO_WORLD_FACTOR;
    const auto screenRatio = Engine::GetScreen()->GetRatio();

    auto rectangle = Rectangle
    {
        {int((float)coordinates.x - zoomFactor * screenRatio), int((float)coordinates.y - zoomFactor)},
        {int((1.0f + zoomFactor) * 2.0f * screenRatio), int((1.0f + zoomFactor) * 2.0f)}    
    };

    utility::Clamp(rectangle.Position.x, 0, worldMap->GetTiles().GetWidth() - 1);
    utility::Clamp(rectangle.Position.y, 0, worldMap->GetTiles().GetHeight() - 1);

    utility::Clamp(rectangle.Size.x, 0, worldMap->GetTiles().GetWidth());
    utility::Clamp(rectangle.Size.y, 0, worldMap->GetTiles().GetHeight());

    return rectangle;
}