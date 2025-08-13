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

#include "FlumenBattle/Battle/Render/BattleTileModel.h"
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
#include "FlumenBattle/Battle/Render/CombatantModel.h"
#include "FlumenBattle/World/Character/CharacterAction.h"
#include "FlumenBattle/World/Character/Spell.h"
#include "FlumenBattle/Battle/SpellCaster.h"

using namespace battle;
using namespace battle::render;

const Float CAMERA_SHIFT_DURATION = 0.5f;

static Camera* camera = nullptr;

static BattleController * battleController = nullptr;

static HumanController * humanController = nullptr;

static Float shadeTimer = 0.0f;

static const auto BACKGROUND_COLOR = Color(0.1f, 0.1f, 0.7f, 1.0f);

static Pool <Event> lastRenderPassJobs = Pool <Event> (16);

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

    CombatantModel::Get()->Initialize();
}

void BattleTileModel::Initialize()
{   
	RenderManager::SetBackgroundColor(BACKGROUND_COLOR);
}

void BattleTileModel::CreateCamera()
{
    auto screen = Engine::GetScreen();
	camera = new Camera(screen);
	RenderManager::AddCamera(Cameras::BATTLE, camera);

    auto centerTile = battleScene->GetBattleMap()->GetCenterTile();
    camera->SetTarget(Position3(centerTile->Position, 0.0f));
    
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

void BattleTileModel::RenderSpellArea()
{
    if(HumanController::Get()->IsInitiatingTargeting() == false)
        return;

    auto caster = BattleController::Get()->GetSelectedCharacter();
    if(*caster->GetSelectedAction() != world::character::CharacterActions::CAST_SPELL)
        return;

    auto spell = caster->GetSelectedSpell();
    if(spell->HasAreaOfEffect() == false)
        return;

    auto hoveredTile = humanController->GetHoveredTile();
    if(hoveredTile == nullptr)
        return;

    auto &tiles = SpellCaster::GetAffectedTiles(*BattleController::Get()->GetSelectedCombatant(), *spell, hoveredTile);

    shader->Bind();

    for(auto &tile : tiles)
    {
        shader->SetConstant(tile->Position, "hexPosition");

        shader->SetConstant(BattleMap::TILE_SIZE, "hexSize");

        shader->SetConstant(Color::BLUE, "color");

        shader->SetConstant(0.5f, "opacity");

        glDrawArrays(GL_TRIANGLES, 0, 18);
    }

    shader->Unbind();
}

void BattleTileModel::Render() 
{
	shader->Bind();

	shader->SetConstant(camera->GetMatrix(), "viewMatrix");

	shader->SetConstant(BattleMap::TILE_SIZE, "hexSize");

	shader->SetConstant(1.0f, "opacity");

	shader->SetConstant(0.0f, "depth");

    auto battleMap = battleScene->battleMap;
    for(auto tile = battleMap->GetTiles().GetStart(); tile != battleMap->GetTiles().GetEnd(); ++tile)
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

    CombatantModel::Get()->Render();

    RenderPath();

    RenderSpellArea();

    for(auto &job : lastRenderPassJobs)
    {
        job.Invoke();
    }
}

void BattleTileModel::AddRenderJob(Event job)
{
    *lastRenderPassJobs.Add() = job;
}

void BattleTileModel::RemoveRenderJob(Event job)
{
    lastRenderPassJobs.Remove(job);
}