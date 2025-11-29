#include "FlumenEngine/Render/ShaderManager.hpp"
#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Render/Camera.hpp"
#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/DataBuffer.hpp"

#include "TreeModel.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Tile/WorldMap.h"
#include "FlumenBattle/World/Tile/WorldTile.h"
#include "FlumenBattle/World/Tile/WorldBiome.h"

using namespace world::render;

#define MAX_TREE_PER_TILE 10

static Camera* camera = nullptr;

static DataBuffer *positionBuffer = nullptr;

static DataBuffer *colorBuffer = nullptr;

static DataBuffer *scaleBuffer = nullptr;

void TreeModel::Initialize()
{
    auto &scene = *WorldScene::Get();

    auto map = scene.GetWorldMap();

    static auto positions = container::Array <Position2> (map->GetTileCount() * MAX_TREE_PER_TILE);

    static auto colors = container::Array <Float4> (map->GetTileCount() * MAX_TREE_PER_TILE);

    static auto scales = container::Array <Float> (map->GetTileCount() * MAX_TREE_PER_TILE);

    positionBuffer = new DataBuffer(positions.GetMemoryCapacity(), positions.GetStart());

    colorBuffer = new DataBuffer(colors.GetMemoryCapacity(), colors.GetStart());

    scaleBuffer = new DataBuffer(scales.GetMemoryCapacity(), scales.GetStart());

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

            auto treeCount = utility::GetRandom(8, MAX_TREE_PER_TILE);
            auto treeIndex = 0;
            for(int i = 0; i < MAX_TREE_PER_TILE; ++i, ++treeIndex)
            {
                /*if(treeIndex < treeCount)
                {
                    
                }*/

                Position2 position;
                while(true)
                {
                    position = utility::GetRandomPositionAround(tile::WorldMap::WORLD_TILE_SIZE * 0.9f, tile.Position);

                    bool isTooClose = false;
                    for(auto &tree : tileTrees)
                    {
                        auto distance = glm::length(position - tree);
                        if(distance < 13.0f)
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
}

void TreeModel::Render()
{
    camera = RenderManager::GetCamera(Cameras::WORLD);

    auto &scene = *WorldScene::Get();

    auto map = scene.GetWorldMap();

    auto shader = ShaderManager::GetShader("Tree");

    shader->Bind();

    shader->SetConstant(camera->GetMatrix(), "viewMatrix");

	shader->SetConstant(0.2f, "depth");

    shader->SetConstant(15.0f, "treeSize");

    positionBuffer->Bind(0);

    colorBuffer->Bind(1);

    scaleBuffer->Bind(2);

    glDrawArrays(GL_TRIANGLES, 0, 6 * map->GetTileCount() * MAX_TREE_PER_TILE);

    shader->Unbind();
}