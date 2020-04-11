#include "FlumenEngine/Render/ShaderManager.hpp"
#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Render/Camera.hpp"
#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Core/SceneManager.hpp"

#include "FlumenBattle/BattleTileModel.h"
#include "FlumenBattle/BattleScene.h"
#include "FlumenBattle/BattleMap.h"
#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/Character.h"

#define BATTLE_TILE_SIZE 30.0f

const Float4 DEFAULT_TILE_COLOR = Float4(1.0f, 0.5f, 0.5f, 1.0f);

const Float4 NEARBY_TILE_COLOR = Float4(0.75f, 0.375f, 0.375f, 1.0f);

const Float4 HOVERED_TILE_COLOR = Float4(0.5f, 0.25f, 0.25f, 1.0f);

Camera* camera = nullptr;

const Array <BattleTile*> & BattleTile::GetNearbyTiles(Integer range)
{
    return Map->GetNearbyTiles(this, range);
}

void BattleTileModel::Initialize()
{
    shader = ShaderManager::GetShader("Hex");

    battleScene = (BattleScene*)SceneManager::Get(Scenes::BATTLE);

    camera = RenderManager::GetCamera(Cameras::BATTLE);

    auto centerTile = battleScene->GetBattleMap()->GetCenterTile();
    camera->SetTarget(Position3(centerTile->Position, 0.0f));

    camera->Zoom(0.7f);
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
    auto character = battleScene->GetSelectedCharacter();

    if(character != nullptr)
    {
        const auto &nearbyTiles = map->GetNearbyTiles(character->GetTile(), character->GetMovement());
        for(auto tileIterator = nearbyTiles.GetStart(); tileIterator != nearbyTiles.GetEnd(); ++tileIterator)
        {
            auto tile = *tileIterator;
            if(tile == nullptr)
                continue;

            shader->SetConstant(tile->Position, "hexPosition");

            shader->SetConstant(BATTLE_TILE_SIZE * 0.7f, "hexSize");

            shader->SetConstant(NEARBY_TILE_COLOR, "color");

            glDrawArrays(GL_TRIANGLES, 0, 18);
        }
    }

    auto hoveredTile = battleScene->hoveredTile;
    if(hoveredTile != nullptr)
    {
        shader->SetConstant(hoveredTile->Position, "hexPosition");

        shader->SetConstant(BATTLE_TILE_SIZE * 0.5f, "hexSize");

        shader->SetConstant(HOVERED_TILE_COLOR, "color");

        glDrawArrays(GL_TRIANGLES, 0, 18);
    }

	shader->Unbind();
}