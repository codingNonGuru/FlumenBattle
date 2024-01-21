#include "FlumenCore/Observer.h"
#include "FlumenCore/Time.hpp"

#include "FlumenEngine/Render/ShaderManager.hpp"
#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Render/Camera.hpp"
#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/MeshManager.hpp"
#include "FlumenEngine/Render/Mesh.hpp"
#include "FlumenEngine/Core/Engine.hpp"
#include "FlumenEngine/Render/DataBuffer.hpp"
#include "FlumenEngine/Interface/Sprite.hpp"

#include "FlumenBattle/Battle/BattleTileModel.h"
#include "FlumenBattle/Battle/BattleScene.h"
#include "FlumenBattle/Battle/BattleController.h"
#include "FlumenBattle/Battle/HumanController.h"
#include "FlumenBattle/Battle/BattleMap.h"
#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/Battle/Combatant.h"
#include "FlumenBattle/Battle/BattleTile.h"

using namespace battle;

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

static DataBuffer *positionBuffer = nullptr;

static const auto MAXIMUM_TILES_PER_RANGE_RING = 1024;

void BattleTileModel::RenderActionRange()
{
    if(HumanController::Get()->IsInitiatingTargeting() == false)
        return;

    static auto positions = container::Array <Position2> (MAXIMUM_TILES_PER_RANGE_RING);

    static auto rangeShader = ShaderManager::GetShader("MassSprite");

    static Sprite *dotSprite = new Sprite(rangeShader, "Dot");

    if(positionBuffer == nullptr)
    {
        positionBuffer = new DataBuffer(positions.GetMemoryCapacity(), positions.GetStart());
    }

    auto combatant = BattleController::Get()->GetSelectedCombatant();

    auto actionRange = combatant->GetCharacter()->GetActionRange();

    auto &nearbyTiles = combatant->GetTile()->GetTileRing(actionRange);

    positions = container::Array <Position2> (nearbyTiles.GetSize());

    for(auto &tile : nearbyTiles)
    {
        *positions.Add() = tile->Position;
    }

    positionBuffer->UploadData(positions.GetStart(), positions.GetMemorySize());

    rangeShader->Bind();

    rangeShader->SetConstant(camera->GetMatrix(), "viewMatrix");

	rangeShader->SetConstant(0.0f, "depth");

    rangeShader->SetConstant(1.0f, "opacity");

    rangeShader->SetConstant(25.0f, "spriteSize");

    positionBuffer->Bind(0);

    dotSprite->BindDefaultTextures();

    glDrawArrays(GL_TRIANGLES, 0, 6 * nearbyTiles.GetSize());

    rangeShader->Unbind();
}

void BattleTileModel::Render() 
{
	shader->Bind();

	shader->SetConstant(camera->GetMatrix(), "viewMatrix");

	shader->SetConstant(BattleMap::TILE_SIZE, "hexSize");

	shader->SetConstant(1.0f, "opacity");

	shader->SetConstant(0.0f, "depth");

    auto battleMap = battleScene->battleMap;
    for(auto tile = battleMap->tiles.GetStart(); tile != battleMap->tiles.GetEnd(); ++tile)
    {
        shader->SetConstant(tile->Position, "hexPosition");

        shader->SetConstant(BattleMap::TILE_SIZE, "hexSize");

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

        //const auto &nearbyTiles = map->GetNearbyTiles(combatant->GetTile(), combatant->GetMovement());
        const auto &nearbyTiles = combatant->GetTile()->FindPath(combatant->GetMovement());
        for(auto tileIterator = nearbyTiles.GetStart(); tileIterator != nearbyTiles.GetEnd(); ++tileIterator)
        {
            auto tile = tileIterator->Tile;
            if(tile == nullptr)
                continue;

            shader->SetConstant(tile->Position, "hexPosition");

            shader->SetConstant(BattleMap::TILE_SIZE * 0.65f, "hexSize");

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

            shader->SetConstant(BattleMap::TILE_SIZE * 0.65f, "hexSize");

            shader->SetConstant(Color::BLACK, "color");

            shader->SetConstant(0.1f, "opacity");

            glDrawArrays(GL_TRIANGLES, 0, 18);
        }
    }

	shader->Unbind();

    RenderActionRange();
}