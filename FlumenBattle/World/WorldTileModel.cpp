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

#define WORLD_TILE_SIZE 34.6666f

const Float CAMERA_SHIFT_DURATION = 0.5f;

static Camera* camera = nullptr;

static Float shadeTimer = 0.0f;

static Sprite *groupSprite = nullptr;

static Sprite *bootSprite = nullptr;

static Sprite *dotSprite = nullptr;

using namespace world;

static WorldController * worldController = nullptr;

WorldTileModel::WorldTileModel()
{
    auto hexMesh = MeshManager::GetMeshes().Add("Hex"); 
	*hexMesh = Mesh::GenerateHex();

    shader = ShaderManager::GetShader("Hex");

    groupShader = ShaderManager::GetShader("Sprite");

    groupSprite = new Sprite(nullptr, groupShader);

    bootSprite = new Sprite(TextureManager::GetTexture("TravelBoot"), groupShader); 

    dotSprite = new Sprite(TextureManager::GetTexture("Dot"), groupShader); 

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

        shader->SetConstant(tile->Shade, "color");

        glDrawArrays(GL_TRIANGLES, 0, 18);
    }

    /*auto map = worldScene->GetWorldMap();
    auto combatant = battleController->GetSelectedCombatant();

    if(combatant != nullptr)
    {
        shadeTimer += Time::GetDelta();
        auto timeFactor = 1.0f + sin(shadeTimer * 6.0f);
        timeFactor = 0.05f + timeFactor * 0.05f;
        shader->SetConstant(timeFactor, "opacity");

        const auto &nearbyTiles = map->GetNearbyTiles(combatant->GetTile(), combatant->GetMovement());
        for(auto tileIterator = nearbyTiles.GetStart(); tileIterator != nearbyTiles.GetEnd(); ++tileIterator)
        {
            auto tile = *tileIterator;
            if(tile == nullptr)
                continue;

            shader->SetConstant(tile->Position, "hexPosition");

            shader->SetConstant(BATTLE_TILE_SIZE * 0.65f, "hexSize");

            shader->SetConstant(Color::BLACK, "color");

            glDrawArrays(GL_TRIANGLES, 0, 18);
        }
    }*/

    auto hoveredTile = worldController->GetHoveredTile();
    if(hoveredTile != nullptr && worldScene->GetPlayerGroup()->ValidateAction(GroupActions::TRAVEL, {hoveredTile}))
    {
        bootSprite->Draw(camera, {hoveredTile->Position, Scale2(0.3f, 0.3f), Opacity(1.0f), DrawOrder(-2)});
    }

	shader->Unbind();

    for(auto group : *worldScene->groups)
    {
        auto position = group.GetTile()->Position;
        if(group.GetAction() != nullptr && group.GetAction()->Type == GroupActions::TRAVEL)
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