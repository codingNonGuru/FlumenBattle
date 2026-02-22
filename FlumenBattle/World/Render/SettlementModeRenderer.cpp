#include "FlumenEngine/Render/ShaderManager.hpp"
#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Render/Camera.hpp"
#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/TextureManager.hpp"
#include "FlumenEngine/Interface/Sprite.hpp"
#include "FlumenEngine/Render/HexRenderer.h"

#include "SettlementModeRenderer.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Tile/WorldMap.h"
#include "FlumenBattle/World/WorldController.h"
#include "FlumenBattle/World/Tile/WorldTile.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/SettlementTile.h"
#include "FlumenBattle/World/Settlement/TileImprovement.h"
#include "FlumenBattle/World/Group/GroupCore.h"
#include "FlumenBattle/World/Polity/HumanMind.h"
#include "FlumenBattle/Config.h"

using namespace world::render;

#define WORLD_TILE_SIZE tile::WorldMap::WORLD_TILE_SIZE

static Shader *shader = nullptr;

static Shader *groupShader = nullptr;

static Camera *camera = nullptr;

SettlementModeRenderer::SettlementModeRenderer()
{
    shader = ShaderManager::GetShader("Hex");

    groupShader = ShaderManager::GetShader("Sprite");

    camera = RenderManager::GetCamera(Cameras::WORLD);
}

void SettlementModeRenderer::RenderBorderExpansionMap()
{
    if(WorldController::Get()->IsBorderExpandActive() == false)
        return;

    static const auto playerGroup = WorldScene::Get()->GetPlayerGroup();
    const auto playerSettlement = WorldScene::Get()->GetPlayerSettlement();

    static const auto BORDER_EXPANSION_MAX_DISTANCE = engine::ConfigManager::Get()->GetValue(game::ConfigValues::BORDER_EXPANSION_MAX_DISTANCE).Integer;

    const auto playerTile = playerGroup->GetTile();
    const auto &nearbyTiles = playerTile->GetNearbyTiles(BORDER_EXPANSION_MAX_DISTANCE);

    shader->Bind();

    shader->SetConstant(camera->GetMatrix(), "viewMatrix");

	shader->SetConstant(0.5f, "depth");

    shader->SetConstant(0.7f, "opacity");

    shader->SetConstant(WORLD_TILE_SIZE, "hexSize");

    auto hasAtLeastOneNeighbour = [&] (tile::WorldTile *tile)
    {
        bool hasAtLeastOneNeighbour = false;

        auto immediateNeighbours = tile->GetNearbyTiles();
        for(auto &neighbour : immediateNeighbours)
        {
            if(neighbour->GetOwner() == playerSettlement)
            {
                hasAtLeastOneNeighbour = true;
                break;
            }
        }

        return hasAtLeastOneNeighbour;
    };

    for(auto &tile : nearbyTiles.Tiles)
    {
        if(tile->IsOwned() == true)
            continue;

        auto color = [&]
        {
            if(hasAtLeastOneNeighbour(tile) == true)
            {
                bool canAffordToExpand = playerSettlement->CanAffordToExpandHere(tile);
                bool hasExplored = playerSettlement->HasExplored(tile);

                if(canAffordToExpand == true && hasExplored == true)
                    return Color::GREEN;
                else if(canAffordToExpand == true || hasExplored == true)
                    return Color::YELLOW;
                else
                    return Color::ORANGE;
            }
            else
                return Color::RED;
        } ();

        shader->SetConstant(tile->Position, "hexPosition");

        shader->SetConstant(color, "color");

        glDrawArrays(GL_TRIANGLES, 0, 18);
    }

    shader->Unbind();

    auto &finishedExplorations = playerSettlement->GetFinishedExplorations();

    for(auto &exploration : finishedExplorations)
    {
        static const auto mapSprite = new Sprite(groupShader, ::render::TextureManager::GetTexture("MapStroked"));

        mapSprite->Draw(camera, {exploration.Tile->Position, Scale2(0.7f), Opacity(1.0f), DrawOrder(-2)});
    }

    auto currentExploration = playerSettlement->GetCurrentlyExploredTile();
    if(currentExploration != nullptr)
    {
        static const auto backpackSprite = new Sprite(groupShader, ::render::TextureManager::GetTexture("BackpackStroked"));

        backpackSprite->Draw(camera, {currentExploration->Position, Scale2(0.7f), Opacity(1.0f), DrawOrder(-2)});
    }

    auto hoveredTile = WorldController::Get()->GetHoveredTile();
    if(hoveredTile != nullptr && hasAtLeastOneNeighbour(hoveredTile) == true && playerSettlement->HasExplored(hoveredTile) == true && playerSettlement->CanAffordToExpandHere(hoveredTile) == true)
    {
        engine::render::HexRenderer::RenderEmptyHex(camera, hoveredTile->Position, WORLD_TILE_SIZE, 0.7f, Color::WHITE, 0.7f);
    }
}

void SettlementModeRenderer::RenderSettleModeMap()
{
    if(WorldController::Get()->IsSettleModeActive() == false)
        return;

    static const auto playerGroup = WorldScene::Get()->GetPlayerGroup();
    const auto playerSettlement = WorldScene::Get()->GetPlayerSettlement();

    const auto playerTile = playerGroup->GetTile();

    shader->Bind();

    shader->SetConstant(camera->GetMatrix(), "viewMatrix");

	shader->SetConstant(0.5f, "depth");

    shader->SetConstant(0.7f, "opacity");

    shader->SetConstant(WORLD_TILE_SIZE, "hexSize");

    for(int i = MINIMUM_COLONIZATION_RANGE; i <= MAXIMUM_COLONIZATION_RANGE; ++i)
    {
        auto tileRing = playerTile->GetTileRing(i);
        for(auto &tile : tileRing)
        {
            if(tile->IsBorderingOwnedTile() == true)
                continue;

            if(tile->HasRelief(WorldReliefs::SEA) == true)
                continue;

            shader->SetConstant(tile->Position, "hexPosition");

            auto color = [&]
            {
                if(playerSettlement->HasAnySettlers() == true)
                {
                    return Color::GREEN;
                }   
                else
                {
                    return Color::YELLOW;
                }
            } ();

            shader->SetConstant(color, "color");

            glDrawArrays(GL_TRIANGLES, 0, 18);
        }
    }

    auto settleTarget = polity::HumanMind::Get()->GetSettleTarget(playerSettlement);
    if(settleTarget == nullptr)
        return;

    static const auto alphaSpriteShader = ShaderManager::GetShader("AlphaSprite");

    static const auto bannerFrameCore = new Sprite(alphaSpriteShader, ::render::TextureManager::GetTexture("BannerCore"));

    const auto banner = playerSettlement->GetBanner();
    bannerFrameCore->SetColor(&banner);

    bannerFrameCore->Draw(camera, {settleTarget->Position + Position2(0.0f, -WORLD_TILE_SIZE * 0.5f), Scale2(1.0f), Opacity(1.0f), DrawOrder(-2)});

    static const auto bannerFrameSprite = new Sprite(groupShader, ::render::TextureManager::GetTexture("BannerFrame"));

    bannerFrameSprite->Draw(camera, {settleTarget->Position + Position2(0.0f, -WORLD_TILE_SIZE * 0.5f), Scale2(1.0f), Opacity(1.0f), DrawOrder(-2)});

    auto hoveredTile = WorldController::Get()->GetHoveredTile();
    if(hoveredTile != nullptr && hoveredTile->IsBorderingOwnedTile() == false && hoveredTile->HasRelief(WorldReliefs::SEA) == false && playerSettlement->HasAnySettlers() == true)
    {
        engine::render::HexRenderer::RenderEmptyHex(camera, hoveredTile->Position, WORLD_TILE_SIZE, 0.7f, Color::WHITE, 0.7f);
    }
}

void SettlementModeRenderer::RenderExploreMap()
{
    if(WorldController::Get()->IsExploreModeActive() == false)
        return;

    static const auto playerGroup = WorldScene::Get()->GetPlayerGroup();
    const auto playerSettlement = WorldScene::Get()->GetPlayerSettlement();

    const auto playerTile = playerGroup->GetTile();
    const auto &nearbyTiles = playerTile->GetNearbyTiles(MAXIMUM_COLONIZATION_RANGE);

    shader->Bind();

    shader->SetConstant(camera->GetMatrix(), "viewMatrix");

	shader->SetConstant(0.5f, "depth");

    shader->SetConstant(0.7f, "opacity");

    shader->SetConstant(WORLD_TILE_SIZE, "hexSize");

    for(auto &tile : nearbyTiles.Tiles)
    {
        if(playerSettlement->CanExploreHere(tile) == false)
            continue;

        if(playerSettlement->IsExploring(tile) == true)
            continue;

        if(playerSettlement->HasExplored(tile) == true)
            continue;

        shader->SetConstant(tile->Position, "hexPosition");

        auto color = [&]
        {
            /*if(playerSettlement->HasAnySettlers() == true)
            {*/
                return Color::GREEN;
            /*}   
            else
            {
                return Color::YELLOW;
            }*/
        } ();

        shader->SetConstant(color, "color");

        glDrawArrays(GL_TRIANGLES, 0, 18);
    }

    shader->Unbind();

    auto &finishedExplorations = playerSettlement->GetFinishedExplorations();

    for(auto &exploration : finishedExplorations)
    {
        static const auto mapSprite = new Sprite(groupShader, ::render::TextureManager::GetTexture("MapStroked"));

        mapSprite->Draw(camera, {exploration.Tile->Position, Scale2(0.7f), Opacity(1.0f), DrawOrder(-2)});
    }

    auto currentExploration = playerSettlement->GetCurrentlyExploredTile();
    if(currentExploration != nullptr)
    {
        static const auto backpackSprite = new Sprite(groupShader, ::render::TextureManager::GetTexture("BackpackStroked"));

        backpackSprite->Draw(camera, {currentExploration->Position, Scale2(0.7f), Opacity(1.0f), DrawOrder(-2)});
    }

    auto hoveredTile = WorldController::Get()->GetHoveredTile();
    if(hoveredTile != nullptr && playerSettlement->CanExploreHere(hoveredTile) == true && playerSettlement->IsExploring(hoveredTile) == false && playerSettlement->HasExplored(hoveredTile) == false)
    {
        engine::render::HexRenderer::RenderEmptyHex(camera, hoveredTile->Position, WORLD_TILE_SIZE, 0.7f, Color::WHITE, 0.7f);
    }
}

void SettlementModeRenderer::RenderTileDevelopMap()
{
    static const auto playerGroup = WorldScene::Get()->GetPlayerGroup();
    const auto playerSettlement = WorldScene::Get()->GetPlayerSettlement();

    const auto playerTile = playerGroup->GetTile();

    shader->Bind();

    shader->SetConstant(camera->GetMatrix(), "viewMatrix");

	shader->SetConstant(0.5f, "depth");

    shader->SetConstant(0.7f, "opacity");

    shader->SetConstant(WORLD_TILE_SIZE, "hexSize");

    shader->SetConstant(Color::GREEN, "color");

    if(WorldController::Get()->IsTileDevelopModeActive() == false)
    {
        auto data = polity::HumanMind::Get()->GetHoveredBuildingQueueItem();

        if(data.Tile != nullptr)
        {
            shader->SetConstant(data.Tile->Position, "hexPosition");

            glDrawArrays(GL_TRIANGLES, 0, 18);
        
            static const auto improvementSprite = new Sprite(groupShader, ::render::TextureManager::GetTexture("FarmImprovement"));

            auto improvement = settlement::TileImprovements(data.Index);
            auto type = settlement::TileImprovementFactory::Get()->BuildImprovementType(improvement);

            improvementSprite->SetTexture(type->TextureName);

            improvementSprite->DrawStandalone(camera, {data.Tile->Position, Scale2(1.0f), Opacity(1.0f), DrawOrder(15)});
        }
    }
    else
    {
        auto improvement = polity::HumanMind::Get()->GetProposedImprovement();

        for(auto &tile : playerSettlement->GetTiles())
        {
            if(playerSettlement->CanImproveHere(tile.Tile, improvement) == false)
                continue;

            if(polity::HumanMind::Get()->IsTileQueuedForImprovement(tile.Tile) == true)
                continue;

            shader->SetConstant(tile.Tile->Position, "hexPosition");

            glDrawArrays(GL_TRIANGLES, 0, 18);
        }
    }

    shader->Unbind();

    if(WorldController::Get()->IsTileDevelopModeActive() == true)
    {
        auto improvement = polity::HumanMind::Get()->GetProposedImprovement();

        auto hoveredTile = WorldController::Get()->GetHoveredTile();
        if(hoveredTile != nullptr && playerSettlement->CanImproveHere(hoveredTile, improvement) == true && polity::HumanMind::Get()->IsTileQueuedForImprovement(hoveredTile) == false)
        {
            engine::render::HexRenderer::RenderEmptyHex(camera, hoveredTile->Position, {WORLD_TILE_SIZE * 1.1f, 0.5f}, 0.7f, Color::WHITE, 0.7f);
        }
    }
}