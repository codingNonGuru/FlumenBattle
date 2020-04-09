#include "FlumenEngine/Render/ShaderManager.hpp"
#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Render/Camera.hpp"
#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Core/SceneManager.hpp"

#include "FlumenBattle/BattleTileModel.h"
#include "FlumenBattle/BattleScene.h"
#include "FlumenBattle/BattleMap.h"
#include "FlumenBattle/Types.hpp"

#define BATTLE_TILE_SIZE 30.0f
#define BATTLE_TILE_DISTANCING 60.0f
#define BATTLE_MAP_OFFSET Position2(400.0f)

Camera* camera = nullptr;

void BattleTileModel::Initialize()
{
    shader = ShaderManager::GetShader("Hex");

    battleScene = SceneManager::Get(Scenes::BATTLE);

    camera = RenderManager::GetCamera(Cameras::BATTLE);
    //camera->Translate(Direction3(0.1f, 0.0f, 0.0f));
    auto centerTile = battleScene->GetBattleMap()->GetCenterTile();
    camera->SetTarget(Position3(centerTile->Position * BATTLE_TILE_DISTANCING, 0.0f));

    camera->Zoom(0.5f);
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
        shader->SetConstant(tile->Position * BATTLE_TILE_DISTANCING, "hexPosition");

        shader->SetConstant(BATTLE_TILE_SIZE, "hexSize");

        shader->SetConstant(Float4(1.0f, 0.5f, 0.5f, 1.0f), "color");

        glDrawArrays(GL_TRIANGLES, 0, 18);

        shader->SetConstant(BATTLE_TILE_SIZE * 0.7f, "hexSize");

        shader->SetConstant(Float4(0.5f, 0.25f, 0.25f, 1.0f), "color");

        glDrawArrays(GL_TRIANGLES, 0, 18);
    }

	shader->Unbind();
}