#include "FlumenCore/Observer.h"
#include "FlumenCore/Time.hpp"

#include "FlumenEngine/Render/ShaderManager.hpp"
#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Render/Camera.hpp"
#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/MeshManager.hpp"
#include "FlumenEngine/Render/Mesh.hpp"
#include "FlumenEngine/Interface/Sprite.hpp"
#include "FlumenEngine/Render/TextureManager.hpp"
#include "FlumenEngine/Render/DataBuffer.hpp"
#include "FlumenEngine/Core/Engine.hpp"
#include "FlumenEngine/Render/Screen.hpp"
#include "FlumenEngine/Render/HexRenderer.h"
#include "FlumenEngine/Render/LineRenderer.h"
#include "FlumenEngine/Render/BufferManager.hpp"
#include "FlumenEngine/Render/FrameBuffer.hpp"

#include "FlumenBattle/World/Render/WorldTileModel.h"
#include "FlumenBattle/World/Render/BorderModel.h"
#include "FlumenBattle/World/Render/FarmModel.h"
#include "FlumenBattle/World/Render/RendererAllocator.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/WorldController.h"
#include "FlumenBattle/World/Tile/WorldMap.h"
#include "FlumenBattle/World/Group/GroupCore.h"
#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/Battle/Combatant.h"
#include "FlumenBattle/World/Tile/WorldTile.h"
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
#include "FlumenBattle/World/Render/RiverModel.h"
#include "FlumenBattle/World/Render/TreeModel.h"
#include "FlumenBattle/World/Render/OceanModel.h"
#include "FlumenBattle/World/Render/MountainRenderer.h"
#include "FlumenBattle/World/Render/SettlementModel.h"
#include "FlumenBattle/World/Render/GroupModel.h"
#include "FlumenBattle/World/Render/RoadModel.h"
#include "FlumenBattle/World/Render/SettlementModeRenderer.h"
#include "FlumenBattle/World/Render/TerrainRenderer.h"
#include "FlumenBattle/Config.h"

#define WORLD_TILE_SIZE tile::WorldMap::WORLD_TILE_SIZE

const Float CAMERA_SHIFT_DURATION = 0.5f;

static Camera* camera = nullptr;

static Float shadeTimer = 0.0f;

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

    bootSprite = new Sprite(groupShader, ::render::TextureManager::GetTexture("TravelBoot")); 

    metalSprite = new Sprite(groupShader, ::render::TextureManager::GetTexture("MetalOre"));

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

    TreeModel::Get()->Initialize();

    OceanModel::Get()->Initialize();

    MountainRenderer::Get()->Initialize();

    RoadModel::Get()->Initialize();

    RiverModel::Get()->Initialize();

    SettlementModel::Get()->Initialize();

    TerrainRenderer::Get()->Initialize();
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

void WorldTileModel::RenderGlobalLight()
{    
    /*auto map = worldScene->GetWorldMap();

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

    hexShader->Unbind();*/
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

void WorldTileModel::RenderFogOfWar()
{
    /*static const auto map = worldScene->GetWorldMap();

    static DataBuffer *tileIndexBuffer = nullptr;

    static auto tileIndices = container::Array <unsigned int> (map->GetTileCount());

    if(tileIndexBuffer == nullptr)
    {
        tileIndexBuffer = new DataBuffer(tileIndices.GetMemoryCapacity(), tileIndices.GetStart());
    }

    tileIndices.Reset();

    const auto frustum = WorldTileModel::Get()->GetFrustum();

    for(auto x = frustum.Position.x; x <= frustum.Position.x + frustum.Size.x; ++x)
    {
        for(auto y = frustum.Position.y - 5; y <= frustum.Position.y + frustum.Size.y + 6; ++y)
        {
            auto tile = map->GetTile(Integer2{x, y});
            if(tile == nullptr)
                continue;

            if(tile->IsRevealed() == true)
                continue;

            auto index = tile - map->GetTiles().GetStart();
            *tileIndices.Add() = index;
        }   
    }

    tileIndexBuffer->UploadData(tileIndices.GetStart(), tileIndices.GetMemorySize());

    static const auto fogOfWarShader = ShaderManager::GetShader("FogOfWar");

    fogOfWarShader->Bind();

    fogOfWarShader->SetConstant(camera->GetMatrix(), "viewMatrix");

	fogOfWarShader->SetConstant(0.0f, "depth");

    fogOfWarShader->SetConstant(WORLD_TILE_SIZE, "hexSize");

    positionBuffer->Bind(0);

    tileIndexBuffer->Bind(1);

    glDrawArrays(GL_TRIANGLES, 0, 18 * tileIndices.GetSize());

    fogOfWarShader->Unbind();*/
}

void WorldTileModel::Render() 
{
    static auto index = 0;
    static auto durationSum = 0;

    auto startClock = high_resolution_clock::now();

    auto map = worldScene->GetWorldMap();

    TerrainRenderer::Get()->Render();

    //OceanModel::Get()->Render();

    FarmModel::Get()->Render();

    //RiverModel::Get()->Render();

    RoadModel::Get()->Render();

    TreeModel::Get()->RenderShadows();

    SettlementModel::Get()->RenderShadows();

    TreeModel::Get()->Render();

    MountainRenderer::Get()->Render();

    BorderModel::Get()->Render();

    //RenderPoliticalOverlay();

    //RenderInterestMap();

    SettlementModel::Get()->Render();

    RenderFogOfWar();

    if(WorldController::Get()->ShouldDisplayMetal() == true)
    {
        auto map = worldScene->GetWorldMap();
        for(auto tile = map->GetTiles().GetStart(); tile != map->GetTiles().GetEnd(); ++tile)
        {
            if(tile->GetResource(settlement::ResourceTypes::ORE) == 0)
                continue;

            metalSprite->DrawStandalone(camera, {tile->Position, Scale2(1.0f), Opacity(1.0f), DrawOrder(10)});
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
                    sprite->DrawStandalone(camera, {tile->Position + positionOffset, Scale2(0.75f), Opacity(1.0f), DrawOrder(10)});
                }
            }
        }
    }

    //RenderGroupSightings();

    RenderImprovements();

    if(worldController->GetHoveredTile() != nullptr)
    {
        engine::render::HexRenderer::RenderEmptyHex(camera, worldController->GetHoveredTile()->Position, WORLD_TILE_SIZE, 0.5f, Color::WHITE, 0.5f);
    }

    SettlementModeRenderer::Get()->RenderBorderExpansionMap();

    SettlementModeRenderer::Get()->RenderSettleModeMap();

    SettlementModeRenderer::Get()->RenderExploreMap();

    SettlementModeRenderer::Get()->RenderTileDevelopMap();

    GroupModel::Get()->Render();

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
    const auto targetTile = worldMap->GetTileFromPosition(Float2{targetPosition.x, targetPosition.y});

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