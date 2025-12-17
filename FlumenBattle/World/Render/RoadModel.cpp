#include "FlumenEngine/Render/ShaderManager.hpp"
#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Render/Camera.hpp"
#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/DataBuffer.hpp"
#include "FlumenEngine/Container/HexGrid.h"

#include "RoadModel.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Tile/WorldMap.h"
#include "FlumenBattle/World/Tile/WorldTile.h"
#include "FlumenBattle/World/Settlement/Path.h"
#include "FlumenBattle/World/Settlement/Settlement.h"

using namespace world::render;

#define OFFSET_RANGE 20.0f

#define ROAD_THICKNESS 3.0f

#define SEGMENT_COUNT 6

#define ROAD_COLOR Color(0.5f, 0.5f, 0.5f, 1.0f)

#define RENDER_OPACITY 1.0f

#define RENDER_DEPTH 0.2f

static bool doesDataNeedRefresh = true;

static bool doesQueueNeedRefresh = true;

static container::HexGrid <PathTileData> roadTiles;

void RoadModel::UpdateData()
{
    if(doesQueueNeedRefresh == false)
    {
        return;
    }

    doesQueueNeedRefresh = false;

    data.Reset();

    for(auto &tile : WorldScene::Get()->GetWorldMap()->GetTiles())
    {
        if(tile.GetLinks().GetSize() == 2)
        {
            tile::WorldTile *tiles[3];

            tiles[0] = &tile;

            auto index = 1;
            for(auto &link : tile.GetLinks())
            {
                auto other = link->GetOtherEnd(&tile);

                tiles[index++] = other;
            }

            Position2 corners[3] =
            {
                tiles[0]->Position + roadTiles.GetTile(tiles[0]->Coordinates)->Offset,
                tiles[1]->Position + roadTiles.GetTile(tiles[1]->Coordinates)->Offset,
                tiles[2]->Position + roadTiles.GetTile(tiles[2]->Coordinates)->Offset
            };

            Position2 positions[3] =
            {
                corners[0] * 0.5f + corners[1] * 0.5f,
                corners[0],
                corners[0] * 0.5f + corners[2] * 0.5f,
            };

            /*auto color = [&segment] () 
            {
                return segment.Type == settlement::RoadTypes::UNTRODDEN ?
                Color(0.9f, 0.7f, 0.6f, 1.0f) * 0.6f :
                Color(0.7f, 0.7f, 0.7f, 1.0f) * 0.6f;
            } ();*/

            *data.Add() = {ROAD_COLOR, {positions[0], positions[1], positions[2]}, ROAD_THICKNESS};
        }
        else
        {
            for(auto &link : tile.GetLinks())
            {
                auto other = link->GetOtherEnd(&tile);

                Position2 corners[2] =
                {
                    other->Position + roadTiles.GetTile(other->Coordinates)->Offset,
                    tile.Position + roadTiles.GetTile(tile.Coordinates)->Offset,
                };

                Position2 positions[3] = 
                {
                    corners[1],
                    corners[1] * 0.75f + corners[0] * 0.25f,
                    corners[1] * 0.5f + corners[0] * 0.5f,
                };

                *data.Add() = {ROAD_COLOR, {positions[0], positions[1], positions[2]}, ROAD_THICKNESS};
            }
        }
    }

    buffer->UploadData(data.GetStart(), data.GetMemoryCapacity());

    //tileQueueBuffer->UploadData(tileQueue.GetStart(), tileQueue.GetMemoryCapacity());
}

void RoadModel::Initialize()
{
    auto map = WorldScene::Get()->GetWorldMap();

    //roadTiles.Initialize(map->GetTiles().GetWidth(), map->GetTiles().GetHeight());

    auto roadTile = roadTiles.Get(0, 0);
    for(auto &tile : map->GetTiles())
    {
        auto range = tile.GetSettlement() != nullptr ? OFFSET_RANGE * 0.3f : OFFSET_RANGE;
        auto offset = utility::GetRandomPositionAround(range);

        roadTile->Tile = &tile;
        roadTile->Offset = offset;

        roadTile++;
    }

    UpdateData();
}

void RoadModel::Render()
{
    UpdateData();

    static const auto camera = RenderManager::GetCamera(Cameras::WORLD);

    static auto shader = ShaderManager::GetShader("Road");
    shader->Bind();

    shader->SetConstant(camera->GetMatrix(), "viewMatrix");

	shader->SetConstant(RENDER_OPACITY, "opacity");

	shader->SetConstant(RENDER_DEPTH, "depth");

    buffer->Bind(0);

    glDrawArrays(GL_TRIANGLES, 0, 6 * SEGMENT_COUNT * data.GetSize());

    shader->Unbind();
}

container::HexGrid <PathTileData> &RoadModel::GetRoadTiles()
{
    return roadTiles;
}