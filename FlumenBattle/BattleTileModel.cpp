#include "FlumenEngine/Render/ShaderManager.hpp"
#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Render/Camera.hpp"
#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/MeshManager.hpp"
#include "FlumenEngine/Render/Mesh.hpp"
#include "FlumenEngine/Core/Engine.hpp"

#include "FlumenBattle/BattleTileModel.h"
#include "FlumenBattle/BattleScene.h"
#include "FlumenBattle/BattleController.h"
#include "FlumenBattle/BattleMap.h"
#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/Character.h"
#include "FlumenBattle/BattleTile.h"

#define BATTLE_TILE_SIZE 34.0f

const Float4 DEFAULT_TILE_COLOR = Float4(0.9f, 0.7f, 0.6f, 1.0f);

const Float4 NEARBY_TILE_COLOR = DEFAULT_TILE_COLOR * 0.8f;

const Float4 HOVERED_TILE_COLOR = DEFAULT_TILE_COLOR * 0.6f;

const Float CAMERA_SHIFT_DURATION = 0.7f;

Camera* camera = nullptr;

static BattleController * battleController = nullptr;

void BattleTileModel::Initialize()
{
    auto hexMesh = MeshManager::GetMeshes().Add("Hex"); 
	*hexMesh = Mesh::GenerateHex();

    shader = ShaderManager::GetShader("Hex");

    battleScene = BattleScene::Get();

    battleController = BattleController::Get();

    battleController->OnCharacterSelected.Add(this, &BattleTileModel::HandleCharacterSelected);
    
    auto backgroundColor = Color(0.1f, 0.1f, 0.7f, 1.0f);
	RenderManager::SetBackgroundColor(backgroundColor);

    CreateCamera();
}

void BattleTileModel::CreateCamera()
{
    auto screen = Engine::GetScreen();
	camera = new Camera(screen);
	RenderManager::AddCamera(Cameras::BATTLE, camera);

    auto centerTile = battleScene->GetBattleMap()->GetCenterTile();
    camera->SetTarget(Position3(centerTile->Position, 0.0f));
    camera->Zoom(0.7f);
}

void BattleTileModel::HandleCharacterSelected()
{
    auto character = battleController->GetSelectedCharacter();
    camera->SetTarget(Position3(character->GetPosition(), 0.0f), CAMERA_SHIFT_DURATION);
}

void BattleTileModel::Render() 
{
	shader->Bind();

	shader->SetConstant(camera->GetMatrix(), "viewMatrix");

	shader->SetConstant(BATTLE_TILE_SIZE, "hexSize");

	shader->SetConstant(1.0f, "opacity");

	shader->SetConstant(0.0f, "depth");

    auto battleMap = battleScene->battleMap;
    for(auto tile = battleMap->tiles.GetStart(); tile != battleMap->tiles.GetEnd(); ++tile)
    {
        shader->SetConstant(tile->Position, "hexPosition");

        shader->SetConstant(BATTLE_TILE_SIZE, "hexSize");

        shader->SetConstant(DEFAULT_TILE_COLOR, "color");

        glDrawArrays(GL_TRIANGLES, 0, 18);
    }

    auto map = battleScene->GetBattleMap();
    auto character = battleController->GetSelectedCharacter();

    if(character != nullptr)
    {
        const auto &nearbyTiles = map->GetNearbyTiles(character->GetTile(), character->GetMovement());
        for(auto tileIterator = nearbyTiles.GetStart(); tileIterator != nearbyTiles.GetEnd(); ++tileIterator)
        {
            auto tile = *tileIterator;
            if(tile == nullptr)
                continue;

            shader->SetConstant(tile->Position, "hexPosition");

            shader->SetConstant(BATTLE_TILE_SIZE * 0.8f, "hexSize");

            shader->SetConstant(NEARBY_TILE_COLOR, "color");

            glDrawArrays(GL_TRIANGLES, 0, 18);
        }
    }

    if(battleController->IsMoveInitiated())
    {
        auto hoveredTile = battleController->GetHoveredTile();
        if(hoveredTile != nullptr)
        {
            shader->SetConstant(hoveredTile->Position, "hexPosition");

            shader->SetConstant(BATTLE_TILE_SIZE * 0.8f, "hexSize");

            shader->SetConstant(HOVERED_TILE_COLOR, "color");

            glDrawArrays(GL_TRIANGLES, 0, 18);
        }
    }

	shader->Unbind();
}