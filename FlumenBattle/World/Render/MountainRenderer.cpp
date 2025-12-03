#include "FlumenEngine/Render/ShaderManager.hpp"
#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Render/Camera.hpp"
#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/DataBuffer.hpp"
#include "FlumenEngine/Core/Engine.hpp"

#include "MountainRenderer.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Tile/WorldMap.h"
#include "FlumenBattle/World/Tile/WorldTile.h"
#include "FlumenBattle/World/Tile/WorldBiome.h"
#include "FlumenBattle/World/Render/WorldTileModel.h"

using namespace world::render;

#define MOUNTAINS_PER_TILE 2

#define VERTICES_PER_MOUNTAIN_MESH 9

static Camera* camera = nullptr;

static bool doesQueueNeedRefresh = true;

void MountainRenderer::Initialize()
{
    Engine::OnLoopCycleStarted += [] 
    {
        doesQueueNeedRefresh = true;
    };

    auto &scene = *WorldScene::Get();

    auto map = scene.GetWorldMap();

    for(auto &tile : map->GetTiles())
    {
        if(tile.HasRelief(WorldReliefs::MOUNTAINS) == false)
        {
            for(int i = 0; i < MOUNTAINS_PER_TILE; ++i)
            {
                *positions.Add() = Position2();

                *colors.Add();

                *scales.Add() = 0.0f;
            }
        }
        else
        {
            container::SmartBlock <Position2, MOUNTAINS_PER_TILE> tileMountains;

            auto mountainCount = utility::GetRandom(0, MOUNTAINS_PER_TILE);
            auto mountainIndex = 0;
            for(int i = 0; i < MOUNTAINS_PER_TILE; ++i, ++mountainIndex)
            {
                Position2 position;
                while(true)
                {
                    position = utility::GetRandomPositionAround(tile::WorldMap::WORLD_TILE_SIZE * 0.75f, tile.Position);

                    bool isTooClose = false;
                    for(auto &mountain : tileMountains)
                    {
                        auto distance = glm::length(position - mountain);
                        if(distance < 25.0f)
                        {
                            isTooClose = true;
                            break;
                        }
                    }

                    if(isTooClose == false)
                        break;
                }

                *positions.Add() = position - Position2(0.0f, 10.0f);

                auto color = Color::WHITE * utility::GetRandom(0.25f, 0.4f);

                *colors.Add() = color;

                *scales.Add() = utility::GetRandom(0.8f, 1.0f);

                *tileMountains.Add() = position;
            }
        }
    }

    positionBuffer->UploadData(positions.GetStart(), positions.GetMemoryCapacity());

    colorBuffer->UploadData(colors.GetStart(), colors.GetMemoryCapacity());

    scaleBuffer->UploadData(scales.GetStart(), scales.GetMemoryCapacity());

    tileQueueBuffer->UploadData(tileQueue.GetStart(), tileQueue.GetMemoryCapacity());
}

void MountainRenderer::PrepareQueue()
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

void MountainRenderer::Render()
{
    PrepareQueue();

    camera = RenderManager::GetCamera(Cameras::WORLD);

    auto shader = ShaderManager::GetShader("Mountain");

    shader->Bind();

    shader->SetConstant(camera->GetMatrix(), "viewMatrix");

    shader->SetConstant(MOUNTAINS_PER_TILE, "maxMountainCount");

	shader->SetConstant(0.2f, "depth");

    shader->SetConstant(50.0f, "defaultSize");

    positionBuffer->Bind(0);

    colorBuffer->Bind(1);

    scaleBuffer->Bind(2);

    tileQueueBuffer->Bind(3);

    glDrawArrays(GL_TRIANGLES, 0, VERTICES_PER_MOUNTAIN_MESH * tileQueue.GetSize() * MOUNTAINS_PER_TILE);

    shader->Unbind();
}

int MountainRenderer::GetMaximumMountainsPerTile()
{
    return MOUNTAINS_PER_TILE;
}