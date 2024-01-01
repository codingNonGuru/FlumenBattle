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

#include "FlumenBattle/World/WorldTileModel.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/WorldController.h"
#include "FlumenBattle/World/WorldMap.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/Character.h"
#include "FlumenBattle/Combatant.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/Group/GroupAction.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Polity.h"

#define WORLD_TILE_SIZE 34.6666f

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

    //metalSprite = new Sprite(groupShader, {render::TextureManager::GetTexture("FantRpg_assets"), {0.333f, 0.1f}, {0.04166f, 0.1f}});
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

void WorldTileModel::Render() 
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

        shader->SetConstant(WORLD_TILE_SIZE, "hexSize");

        shader->SetConstant(tile->GetShade(), "color");
        //shader->SetConstant(tile->Shade, "color");

        glDrawArrays(GL_TRIANGLES, 0, 18);
    }

    shader->Unbind();

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

    /*for(auto &path : *worldScene->paths)
    {
        const auto &tiles = path.Tiles;

        for(auto tile = tiles.GetFirst(); tile != tiles.GetLast() - 1; ++tile)
        {
            auto nextTile = tile + 1;

            auto link = (*tile)->GetLinkTo(*nextTile);

            auto position = ((*tile)->Position + (*nextTile)->Position) / 2.0f;
            squareShader->SetConstant(position, "hexPosition");

            Scale2 scale = Scale2(WORLD_TILE_SIZE * 1.732f, 5.0f);
            squareShader->SetConstant(scale, "hexSize");

            auto color = [&link] () {
                return link->Type == settlement::RoadTypes::UNTRODDEN ?
                Color(0.9f, 0.7f, 0.5f, 1.0f) * 0.6f :
                Color(0.7f, 0.7f, 0.7f, 1.0f) * 0.6f;
            } ();
            squareShader->SetConstant(color, "color");

            auto orientation = (*tile)->Position - (*nextTile)->Position;
            auto rotation = atan2(orientation.y, orientation.x);
            squareShader->SetConstant(rotation, "rotation");

            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }*/

	squareShader->Unbind();

    shader->Bind();

    shader->SetConstant(camera->GetMatrix(), "viewMatrix");

	shader->SetConstant(0.4f, "opacity");

	shader->SetConstant(0.0f, "depth");

    map = worldScene->GetWorldMap();
    for(auto tile = map->tiles.GetStart(); tile != map->tiles.GetEnd(); ++tile)
    {
        if(tile->IsOwned() == false)
            continue;
        
        shader->SetConstant(tile->Position, "hexPosition");

        shader->SetConstant(WORLD_TILE_SIZE, "hexSize");

        shader->SetConstant(tile->GetOwner()->GetRulerBanner(), "color");
        //shader->SetConstant(tile->Shade, "color");

        //glDrawArrays(GL_TRIANGLES, 0, 18);
    }

    /*auto polity = worldScene->polities.Get(0);
    auto &interestTiles = polity->GetInterestMap().GetTiles();
    for(auto tile = interestTiles.GetStart(); tile != interestTiles.GetEnd(); ++tile)
    {
        if(tile->Owner == nullptr)
            continue;
        
        shader->SetConstant(map->GetTile(tile->Coordinates)->Position, "hexPosition");

        shader->SetConstant(WORLD_TILE_SIZE, "hexSize");

        shader->SetConstant(tile->Owner->GetBanner(), "color");

        glDrawArrays(GL_TRIANGLES, 0, 18);
    }*/

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

    if(WorldController::Get()->ShouldDisplayResources() == true)
    {
        map = worldScene->GetWorldMap();
        for(auto tile = map->tiles.GetStart(); tile != map->tiles.GetEnd(); ++tile)
        {
            if(tile->GetMetal() == 0)
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
    }

    auto destination = worldScene->GetPlayerGroup()->GetDestination();
    if(destination)
    {
        float factor = 0.0f;
        for(int i = 0; i <= 10; i++, factor += 0.1f)
        {
            auto position = destination->Position * factor + worldScene->GetPlayerGroup()->GetTile()->Position * (1.0f - factor);
            dotSprite->Draw(camera, {position, Scale2(0.15f, 0.15f), Opacity(1.0f), DrawOrder(-2)});
        }
    }

    /*for(auto tile = map->tiles.GetStart(); tile != map->tiles.GetEnd(); ++tile)
    {
        if(tile->GetGroup() == nullptr)
            continue;

        groupSprite->Draw(camera, {tile->Position + Position2(0, -10), Scale2(30, 50), Opacity(1.0f), DrawOrder(-1)});
    }*/
}