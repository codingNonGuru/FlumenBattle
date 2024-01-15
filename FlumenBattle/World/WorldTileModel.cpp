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

#include "FlumenBattle/World/WorldTileModel.h"
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
#include "FlumenBattle/World/Polity.h"
#include "FlumenBattle/Utility/Pathfinder.h"

#define WORLD_TILE_SIZE WorldMap::WORLD_TILE_SIZE

const Float CAMERA_SHIFT_DURATION = 0.5f;

static Camera* camera = nullptr;

static Float shadeTimer = 0.0f;

static Sprite *groupSprite = nullptr;

static Sprite *bootSprite = nullptr;

static Sprite *metalSprite = nullptr;

static Sprite *dotSprite = nullptr;

using namespace world;

static WorldController * worldController = nullptr;

WorldTileModel::WorldTileModel()
{
    auto hexMesh = MeshManager::GetMeshes().Add("Hex"); 
	*hexMesh = Mesh::GenerateHex();

    shader = ShaderManager::GetShader("Hex");

    groupShader = ShaderManager::GetShader("Sprite");

    groupSprite = new Sprite(groupShader);

    bootSprite = new Sprite(groupShader, render::TextureManager::GetTexture("TravelBoot")); 

    metalSprite = new Sprite(groupShader, render::TextureManager::GetTexture("Metal"));

    dotSprite = new Sprite(groupShader, render::TextureManager::GetTexture("Dot")); 

    worldScene = WorldScene::Get();

    worldController = WorldController::Get();

    CreateCamera();
}

void WorldTileModel::Initialize()
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
    for(auto tile = map->tiles.GetStart(); tile != map->tiles.GetEnd(); ++tile)
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

void WorldTileModel::RenderBorders()
{
    auto squareShader = ShaderManager::GetShader("Square");
    squareShader->Bind();

    squareShader->SetConstant(camera->GetMatrix(), "viewMatrix");

	squareShader->SetConstant(1.0f, "opacity");

	squareShader->SetConstant(0.0f, "depth");

    auto map = worldScene->GetWorldMap();
    for(auto tile = map->tiles.GetStart(); tile != map->tiles.GetEnd(); ++tile)
    {
        if(tile->IsOwned() == false)
            continue;

        auto nearbyTiles = tile->GetNearbyTiles();
        for(auto &neighbour : nearbyTiles)
        {
            if(neighbour->IsOwned() == true && tile->GetOwner() == neighbour->GetOwner())
                continue;

            bool isHardBorder = neighbour->IsOwned() == false || neighbour->GetOwner()->GetPolity() != tile->GetOwner()->GetPolity();

            auto position = (tile->Position + neighbour->Position) / 2.0f;
            if(isHardBorder == true)
            {
                position = (position + tile->Position * 0.5f) / 1.5f;
            }
            squareShader->SetConstant(position, "hexPosition");
        
            Scale2 scale = Scale2(WORLD_TILE_SIZE * 1.0f, isHardBorder ? 20.0f : 3.0f);
            squareShader->SetConstant(scale, "hexSize");

            auto orientation = tile->Position - neighbour->Position;
            auto rotation = atan2(orientation.y, orientation.x) + 1.5707f;
            squareShader->SetConstant(rotation, "rotation");

            squareShader->SetConstant(tile->GetOwner()->GetRulerBanner(), "color");

            glDrawArrays(GL_TRIANGLES, 0, 6);

            if(isHardBorder)
            {
                for(auto &secondaryNeighbour : nearbyTiles)
                {
                    if(secondaryNeighbour->GetDistanceTo(*neighbour) != 1)
                        continue;

                    if(secondaryNeighbour->IsOwned() == false)
                        continue;

                    if(neighbour->IsOwned() && secondaryNeighbour->GetOwner()->GetPolity() == neighbour->GetOwner()->GetPolity())
                        continue;

                    if(tile->GetOwner()->GetPolity() != secondaryNeighbour->GetOwner()->GetPolity())
                        continue;

                    auto position = (tile->Position + neighbour->Position + secondaryNeighbour->Position) / 3.0f;
                    position = (position + tile->Position * 0.36f + secondaryNeighbour->Position * 0.36f) / 1.72f;
                    squareShader->SetConstant(position, "hexPosition");
                
                    Scale2 scale = Scale2(WORLD_TILE_SIZE * 0.57f, 20.0f);
                    squareShader->SetConstant(scale, "hexSize");

                    auto orientation = tile->Position - secondaryNeighbour->Position;
                    auto rotation = atan2(orientation.y, orientation.x);
                    squareShader->SetConstant(rotation, "rotation");

                    squareShader->SetConstant(tile->GetOwner()->GetRulerBanner(), "color");

                    glDrawArrays(GL_TRIANGLES, 0, 6);
                }
            }
        }
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
    for(auto tile = map->tiles.GetStart(); tile != map->tiles.GetEnd(); ++tile)
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

void WorldTileModel::RenderGlobalLight()
{
    /*shader->Bind();

    shader->SetConstant(camera->GetMatrix(), "viewMatrix");

	shader->SetConstant(0.0f, "depth");

    shader->SetConstant(0.8f, "opacity");

    auto map = worldScene->GetWorldMap();
    auto lightColor = GetGlobalLightColor();
    for(auto tile = map->tiles.GetStart(); tile != map->tiles.GetEnd(); ++tile)
    {
        shader->SetConstant(tile->Position, "hexPosition");

        shader->SetConstant(WORLD_TILE_SIZE, "hexSize");

        shader->SetConstant(lightColor, "color");

        glDrawArrays(GL_TRIANGLES, 0, 18);
    }

    shader->Unbind();*/
    
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
    auto map = worldScene->GetWorldMap();

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

void WorldTileModel::Render() 
{
    auto startClock = high_resolution_clock::now();

    auto map = worldScene->GetWorldMap();

    if(positionBuffer == nullptr)
    {
        static auto positions = container::Array <Position2> (map->GetTileCount());

        static auto colors = container::Array <Color> (map->GetTileCount());

        for(auto tile = map->tiles.GetStart(); tile != map->tiles.GetEnd(); ++tile)
        {
            *positions.Add() = tile->Position;

            *colors.Add() = tile->GetShade();
        }

        positionBuffer = new DataBuffer(positions.GetMemorySize(), positions.GetStart());

        colorBuffer = new DataBuffer(colors.GetMemorySize(), colors.GetStart());
    }

    RenderTilesAdvanced();

    RenderPaths();

    RenderBorders();

    //RenderPoliticalOverlay();

    //RenderInterestMap();

    RenderSettlements();

    RenderGlobalLight();

    if(WorldController::Get()->ShouldDisplayResources() == true)
    {
        auto map = worldScene->GetWorldMap();
        for(auto tile = map->tiles.GetStart(); tile != map->tiles.GetEnd(); ++tile)
        {
            if(tile->GetResource(settlement::ResourceTypes::METAL) == 0)
                continue;

            metalSprite->Draw(camera, {tile->Position, Scale2(1.0f, 1.0f) * 1.5f, Opacity(1.0f), DrawOrder(-2)});
        }
    }

    auto hoveredTile = worldController->GetHoveredTile();
    bool canPlayerTravel = worldScene->GetPlayerGroup()->ValidateAction(group::GroupActions::TRAVEL, {hoveredTile});
    if(hoveredTile != nullptr && canPlayerTravel)
    {
        bootSprite->Draw(camera, {hoveredTile->Position, Scale2(0.3f, 0.3f), Opacity(1.0f), DrawOrder(-2)});
    }

    for(auto &group : *worldScene->groups)
    {
        auto position = group.GetTile()->Position;
        if(group.GetAction() != nullptr && group.GetAction()->Type == group::GroupActions::TRAVEL)
        {
            auto progress = group.GetActionProgress();
            position = position * (1.0f - progress) + group.GetDestination()->Position * progress;
        }
        position += Position2(0, -15);

        groupSprite->Draw(
            camera, 
            {position, Scale2(18, 30), Opacity(1.0f), DrawOrder(-1)}
            );

        for(int i = 0; i < group.travelActionData.PlannedDestinationCount; ++i)
        {
            auto tile = group.travelActionData.Route[i];
            dotSprite->Draw(camera, {tile->Position, Scale2(0.75f, 0.75f), Opacity(0.6f), DrawOrder(-2)});
        }
    }

    auto currentTile = worldScene->GetPlayerGroup()->GetTile();
    hoveredTile = worldController->GetHoveredTile();
    if(hoveredTile != nullptr && hoveredTile != currentTile && currentTile->GetDistanceTo(*hoveredTile) < 8)
    {
        auto pathData = utility::Pathfinder <WorldTile>::Get()->FindPathDjikstra(currentTile, hoveredTile, 5);

        for(auto &tile : pathData.Tiles)
        {
            if(&tile == pathData.Tiles.GetEnd() - 1)
                break;

            auto nextTile = &tile + 1;

            float factor = 0.0f;
            for(int i = 0; i <= 10; i++, factor += 0.1f)
            {
                auto position = tile.Tile->Position * factor + nextTile->Tile->Position * (1.0f - factor);
                dotSprite->Draw(camera, {position, Scale2(0.25f, 0.25f), Opacity(0.6f), DrawOrder(-2)});
            }
        }
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - startClock);
    std::cout <<"world render duration " << duration.count() << "\n";

    /*auto group = [&] ()
    {
        while(true)
        {
            auto randomGroup = worldScene->GetGroups()->GetRandom();
            if(randomGroup != worldScene->GetPlayerGroup())
                return randomGroup;
        }
    } ();*/


    /*auto destination = worldScene->GetPlayerGroup()->GetDestination();
    if(destination)
    {
        float factor = 0.0f;
        for(int i = 0; i <= 10; i++, factor += 0.1f)
        {
            auto position = destination->Position * factor + worldScene->GetPlayerGroup()->GetTile()->Position * (1.0f - factor);
            dotSprite->Draw(camera, {position, Scale2(0.15f, 0.15f), Opacity(1.0f), DrawOrder(-2)});
        }
    }*/
}