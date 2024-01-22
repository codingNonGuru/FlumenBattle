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
#include "FlumenBattle/World/Character/CharacterClass.h"
#include "FlumenBattle/Battle/Combatant.h"
#include "FlumenBattle/Battle/BattleTile.h"
#include "FlumenBattle/Battle/CombatGroup.h"
#include "FlumenBattle/Utility/Pathfinder.h"

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
    camera->EnableDynamicZooming();
}

void BattleTileModel::HandleCharacterSelected()
{
    auto combatant = battleController->GetSelectedCombatant();
    camera->SetTarget(Position3(combatant->GetPosition(), 0.0f), CAMERA_SHIFT_DURATION);
}

static DataBuffer *positionBuffer = nullptr;

static const auto MAXIMUM_TILES_PER_RANGE_RING = 1024;

static const auto RANGE_DOT_SIZE = 25.0f;

void BattleTileModel::RenderActionRange()
{
    if(HumanController::Get()->IsInitiatingTargeting() == false)
        return;

    static auto positions = container::Array <Position2> (MAXIMUM_TILES_PER_RANGE_RING);

    if(positionBuffer == nullptr)
    {
        positionBuffer = new DataBuffer(positions.GetMemoryCapacity(), positions.GetStart());
    }

    static auto rangeShader = ShaderManager::GetShader("MassSprite");

    static Sprite *dotSprite = new Sprite(rangeShader, "Dot");

    auto combatant = BattleController::Get()->GetSelectedCombatant();

    auto actionRange = combatant->GetCharacter()->GetActionRange();

    auto &nearbyTiles = combatant->GetTile()->GetTileRing(actionRange);

    positions.Reset();

    for(auto &tile : nearbyTiles)
    {
        *positions.Add() = tile->Position;
    }

    positionBuffer->UploadData(positions.GetStart(), positions.GetMemorySize());

    rangeShader->Bind();

    rangeShader->SetConstant(camera->GetMatrix(), "viewMatrix");

	rangeShader->SetConstant(0.0f, "depth");

    rangeShader->SetConstant(1.0f, "opacity");

    rangeShader->SetConstant(RANGE_DOT_SIZE, "spriteSize");

    positionBuffer->Bind(0);

    dotSprite->BindDefaultTextures();

    glDrawArrays(GL_TRIANGLES, 0, 6 * nearbyTiles.GetSize());

    rangeShader->Unbind();
}

static DataBuffer *combatantPositionBuffer = nullptr;

static DataBuffer *combatantTextureOffsetBuffer = nullptr;

static const auto MAXIMUM_COMBATANTS_PER_SCENE = 32;

void BattleTileModel::RenderCombatants()
{
    static auto positions = container::Array <Position2> (MAXIMUM_COMBATANTS_PER_SCENE);

    static auto offsets = container::Array <Position2> (MAXIMUM_COMBATANTS_PER_SCENE);

    if(combatantPositionBuffer == nullptr)
    {
        combatantPositionBuffer = new DataBuffer(positions.GetMemoryCapacity(), positions.GetStart());

        combatantTextureOffsetBuffer = new DataBuffer(offsets.GetMemoryCapacity(), offsets.GetStart());
    }

    static auto massShader = ShaderManager::GetShader("ComplexMassSprite");

    static auto unborderedSprite = new Sprite(massShader, "CombatantsComposite");

    static auto borderedSprite = new Sprite(massShader, "CombatantsCompositeBordered");

    positions.Reset();

    offsets.Reset();

    auto playerGroup = BattleScene::Get()->GetPlayerGroup();
    auto computerGroup = BattleScene::Get()->GetComputerGroup();
    for(auto group : {playerGroup, computerGroup})
    {    
        for(auto &combatant : group->GetCombatants())
        {
            auto character = combatant.GetCharacter();

            *positions.Add() = combatant.GetTile()->Position;

            *offsets.Add() = character->GetClass()->TextureData.Offset;
        }
    }

    combatantPositionBuffer->UploadData(positions.GetStart(), positions.GetMemorySize());

    combatantTextureOffsetBuffer->UploadData(offsets.GetStart(), offsets.GetMemorySize());

    massShader->Bind();

    massShader->SetConstant(camera->GetMatrix(), "viewMatrix");

	massShader->SetConstant(0.0f, "depth");

    massShader->SetConstant(1.0f, "opacity");

    massShader->SetConstant(115.0f, "spriteSize");

    combatantPositionBuffer->Bind(0);

    combatantTextureOffsetBuffer->Bind(1);

    unborderedSprite->BindDefaultTextures();

    glDrawArrays(GL_TRIANGLES, 0, 6 * positions.GetSize());

    if(auto selectedCombatant = BattleController::Get()->GetSelectedCombatant(); selectedCombatant != nullptr)
    {
        positions.Reset();

        offsets.Reset();

        *positions.Add() = selectedCombatant->GetTile()->Position;

        *offsets.Add() = selectedCombatant->GetCharacter()->GetClass()->TextureData.Offset;

        combatantPositionBuffer->UploadData(positions.GetStart(), positions.GetMemorySize());

        combatantTextureOffsetBuffer->UploadData(offsets.GetStart(), offsets.GetMemorySize());

        borderedSprite->BindDefaultTextures();

        glDrawArrays(GL_TRIANGLES, 0, 6 * positions.GetSize());
    }

    massShader->Unbind();
}

static DataBuffer *pathPositionBuffer = nullptr;

static const auto PATH_SPRITE_SIZE = 25.0f;

void BattleTileModel::RenderPath()
{
    if(HumanController::Get()->IsMoveInitiated() == false)
        return;

    static auto positions = container::Array <Position2> (HumanController::MAXIMUM_PATH_LENGTH);

    if(pathPositionBuffer == nullptr)
    {
        pathPositionBuffer = new DataBuffer(positions.GetMemoryCapacity(), positions.GetStart());
    }

    static auto rangeShader = ShaderManager::GetShader("MassSprite");

    static Sprite *dotSprite = new Sprite(rangeShader, "Dot");

    auto combatant = BattleController::Get()->GetSelectedCombatant();

    auto &pathTiles = HumanController::Get()->GetHoveredPath();

    positions.Reset();

    auto index = 0;
    for(auto &tile : pathTiles.Tiles)
    {
        if(tile == combatant->GetTile())
            continue;

        *positions.Add() = tile->Position;

        index++;
        if(index == combatant->GetMovement())
            break;
    }

    pathPositionBuffer->UploadData(positions.GetStart(), positions.GetMemorySize());

    rangeShader->Bind();

    rangeShader->SetConstant(camera->GetMatrix(), "viewMatrix");

	rangeShader->SetConstant(0.0f, "depth");

    rangeShader->SetConstant(1.0f, "opacity");

    rangeShader->SetConstant(PATH_SPRITE_SIZE, "spriteSize");

    pathPositionBuffer->Bind(0);

    dotSprite->BindDefaultTextures();

    glDrawArrays(GL_TRIANGLES, 0, 6 * (pathTiles.Tiles.GetSize() - 1));

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

    RenderCombatants();

    RenderPath();
}