#include "FlumenEngine/Render/ShaderManager.hpp"
#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Render/Camera.hpp"
#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/DataBuffer.hpp"
#include "FlumenEngine/Core/Engine.hpp"

#include "TreeModel.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Tile/WorldMap.h"
#include "FlumenBattle/World/Tile/WorldTile.h"
#include "FlumenBattle/World/Tile/WorldBiome.h"
#include "FlumenBattle/World/Render/WorldTileModel.h"

using namespace world::render;

#define MAX_TREE_PER_TILE 16

#define VERTICES_PER_TREE_MESH 6

#define TREE_SIZE 11.0f

static Camera* camera = nullptr;

static bool doesQueueNeedRefresh = true;

void TreeModel::Initialize()
{
    Engine::OnLoopCycleStarted += [] 
    {
        doesQueueNeedRefresh = true;
    };

    auto &scene = *WorldScene::Get();

    auto map = scene.GetWorldMap();

    for(auto &tile : map->GetTiles())
    {
        if(tile.HasBiome(WorldBiomes::WOODS) == false)
        {
            for(int i = 0; i < MAX_TREE_PER_TILE; ++i)
            {
                *positions.Add() = Position2();

                *colors.Add();

                *scales.Add() = 0.0f;
            }
        }
        else
        {
            container::SmartBlock <Position2, MAX_TREE_PER_TILE> tileTrees;

            auto treeCount = utility::GetRandom(12, MAX_TREE_PER_TILE);
            auto treeIndex = 0;
            for(int i = 0; i < MAX_TREE_PER_TILE; ++i, ++treeIndex)
            {
                Position2 position;
                while(true)
                {
                    position = utility::GetRandomPositionAround(tile::WorldMap::WORLD_TILE_SIZE * 0.9f, tile.Position);

                    bool isTooClose = false;
                    for(auto &tree : tileTrees)
                    {
                        auto distance = glm::length(position - tree);
                        if(distance < 10.0f)
                        {
                            isTooClose = true;
                            break;
                        }
                    }

                    if(isTooClose == false)
                        break;
                }

                *positions.Add() = position - Position2(0.0f, 6.0f);

                auto mixFactor = utility::GetRandom(0.6f, 0.9f);
                auto color = Color::GREEN * mixFactor + Color::RED * (1.0f - mixFactor);

                color = Color::AddSaturation(color, utility::GetRandom(-0.2f, 0.2f));
                color = Color::Lighten(color, utility::GetRandom(-0.5f, -0.4f));

                *colors.Add() = color;

                *scales.Add() = utility::GetRandom(0.93f, 1.0f);

                *tileTrees.Add() = position;
            }
        }
    }

    positionBuffer->UploadData(positions.GetStart(), positions.GetMemoryCapacity());

    colorBuffer->UploadData(colors.GetStart(), colors.GetMemoryCapacity());

    scaleBuffer->UploadData(scales.GetStart(), scales.GetMemoryCapacity());

    tileQueueBuffer->UploadData(tileQueue.GetStart(), tileQueue.GetMemoryCapacity());
}

void TreeModel::PrepareQueue()
{
    if(doesQueueNeedRefresh == false)
    {
        return;
    }

    doesQueueNeedRefresh = false;

    static const auto &scene = *WorldScene::Get();

    auto map = scene.GetWorldMap();

    tileQueue.Reset();

    const auto frustum = WorldTileModel::Get()->GetFrustum();

    for(auto x = frustum.Position.x; x <= frustum.Position.x + frustum.Size.x; ++x)
    {
        for(auto y = frustum.Position.y - 5; y <= frustum.Position.y + frustum.Size.y + 6; ++y)
        {
            auto tile = map->GetTileWithinBounds(Integer2{x, y});
            if(tile == nullptr)
                continue;

            auto index = tile - map->GetTiles().GetStart();
            *tileQueue.Add() = index;
        }   
    }

    tileQueueBuffer->UploadData(tileQueue.GetStart(), tileQueue.GetMemoryCapacity());
}

void TreeModel::Render()
{
    PrepareQueue();

    camera = RenderManager::GetCamera(Cameras::WORLD);

    auto shader = ShaderManager::GetShader("Tree");

    shader->Bind();

    shader->SetConstant(camera->GetMatrix(), "viewMatrix");

    shader->SetConstant(MAX_TREE_PER_TILE, "maxTreeCount");

	shader->SetConstant(0.2f, "depth");

    shader->SetConstant(TREE_SIZE, "treeSize");

    positionBuffer->Bind(0);

    colorBuffer->Bind(1);

    scaleBuffer->Bind(2);

    tileQueueBuffer->Bind(3);

    glDrawArrays(GL_TRIANGLES, 0, VERTICES_PER_TREE_MESH * tileQueue.GetSize() * MAX_TREE_PER_TILE);

    shader->Unbind();
}

void TreeModel::RenderShadows()
{
    PrepareQueue();

    camera = RenderManager::GetCamera(Cameras::WORLD);

    auto &scene = *WorldScene::Get();

    auto map = scene.GetWorldMap();

    auto shader = ShaderManager::GetShader("TreeShadow");

    shader->Bind();

    shader->SetConstant(camera->GetMatrix(), "viewMatrix");

    shader->SetConstant(MAX_TREE_PER_TILE, "maxTreeCount");

	shader->SetConstant(0.2f, "depth");

    positionBuffer->Bind(0);

    scaleBuffer->Bind(1);

    tileQueueBuffer->Bind(2);

    glDrawArrays(GL_TRIANGLES, 0, VERTICES_PER_TREE_MESH * tileQueue.GetSize() * MAX_TREE_PER_TILE);

    shader->Unbind();
}

int TreeModel::GetMaximumTreesPerTile()
{
    return MAX_TREE_PER_TILE;
}