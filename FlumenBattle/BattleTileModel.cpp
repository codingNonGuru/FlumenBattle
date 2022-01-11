#include "FlumenCore/Observer.h"
#include "FlumenCore/Time.hpp"

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
#include "FlumenBattle/HumanController.h"
#include "FlumenBattle/BattleMap.h"
#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/Character.h"
#include "FlumenBattle/Combatant.h"
#include "FlumenBattle/BattleTile.h"

#define BATTLE_TILE_SIZE 34.6666f

const Float CAMERA_SHIFT_DURATION = 0.5f;

static Camera* camera = nullptr;

static BattleController * battleController = nullptr;

static HumanController * humanController = nullptr;

static Float shadeTimer = 0.0f;

BattleTileModel::BattleTileModel()
{
    auto hexMesh = MeshManager::GetMeshes().Add("Hex"); 
	*hexMesh = Mesh::GenerateHex();

    shader = ShaderManager::GetShader("Hex");

    battleScene = BattleScene::Get();

    battleController = BattleController::Get();

    battleController->OnCharacterSelected += {this, &BattleTileModel::HandleCharacterSelected};

    humanController = HumanController::Get();

    CreateCamera();
}

void BattleTileModel::Initialize()
{   
    auto backgroundColor = Color(0.1f, 0.1f, 0.7f, 1.0f);
	RenderManager::SetBackgroundColor(backgroundColor);
}

void BattleTileModel::CreateCamera()
{
    auto screen = Engine::GetScreen();
	camera = new Camera(screen);
	RenderManager::AddCamera(Cameras::BATTLE, camera);

    auto centerTile = battleScene->GetBattleMap()->GetCenterTile();
    camera->SetTarget(Position3(centerTile->Position, 0.0f));
    camera->Zoom(2.0f);
}

void BattleTileModel::HandleCharacterSelected()
{
    auto combatant = battleController->GetSelectedCombatant();
    camera->SetTarget(Position3(combatant->GetPosition(), 0.0f), CAMERA_SHIFT_DURATION);
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

        shader->SetConstant(tile->Shade, "color");

        glDrawArrays(GL_TRIANGLES, 0, 18);
    }

    auto map = battleScene->GetBattleMap();
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
    }

    if(humanController->IsMoveInitiated())
    {
        auto hoveredTile = humanController->GetHoveredTile();
        if(hoveredTile != nullptr)
        {
            shader->SetConstant(hoveredTile->Position, "hexPosition");

            shader->SetConstant(BATTLE_TILE_SIZE * 0.65f, "hexSize");

            shader->SetConstant(Color::BLACK, "color");

            shader->SetConstant(0.1f, "opacity");

            glDrawArrays(GL_TRIANGLES, 0, 18);
        }
    }

	shader->Unbind();
}