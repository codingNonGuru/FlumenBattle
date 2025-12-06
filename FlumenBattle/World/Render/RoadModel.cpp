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

#define OFFSET_RANGE 15.0f

#define ROAD_THICKNESS 5.0f

struct PathTileData : core::hex::Tile
{
    world::tile::WorldTile *Tile;

    Position2 Offset;    
};

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

    /*colors.Reset();

    lengths.Reset();

    thicknesses.Reset();

    rotations.Reset();*/

    static auto &pathSegments = WorldScene::Get()->GetPathSegments();
    for(auto &segment : pathSegments)
    {
        auto tile = segment.To;
        auto nextTile = segment.From;

        auto toPosition = tile->Position + roadTiles.GetTile(segment.To->Coordinates)->Offset;
        auto fromPosition = nextTile->Position + roadTiles.GetTile(segment.From->Coordinates)->Offset;

        auto direction = toPosition - fromPosition;

        auto position = (toPosition + fromPosition) * 0.5f;
        //*positions.Add() = position;

        auto length = glm::length(direction);
        //*lengths.Add() = length;

        //*thicknesses.Add() = ROAD_THICKNESS;

        auto color = [&segment] () 
        {
            return segment.Type == settlement::RoadTypes::UNTRODDEN ?
            Color(0.9f, 0.7f, 0.6f, 1.0f) * 0.6f :
            Color(0.7f, 0.7f, 0.7f, 1.0f) * 0.6f;
        } ();
        //*colors.Add() = color;

        auto rotation = atan2(direction.y, direction.x);
        //*rotations.Add() = rotation;

        *data.Add() = {color, position, rotation, length, ROAD_THICKNESS};
    }

    buffer->UploadData(data.GetStart(), data.GetMemoryCapacity());

    /*colorBuffer->UploadData(colors.GetStart(), colors.GetMemoryCapacity());

    lengthBuffer->UploadData(lengths.GetStart(), lengths.GetMemoryCapacity());

    thicknessBuffer->UploadData(thicknesses.GetStart(), thicknesses.GetMemoryCapacity());

    rotationBuffer->UploadData(rotations.GetStart(), rotations.GetMemoryCapacity());*/

    //tileQueueBuffer->UploadData(tileQueue.GetStart(), tileQueue.GetMemoryCapacity());
}

void RoadModel::Initialize()
{
    auto map = WorldScene::Get()->GetWorldMap();

    roadTiles.Initialize(map->GetTiles().GetWidth(), map->GetTiles().GetHeight());

    auto roadTile = roadTiles.Get(0, 0);
    for(auto &tile : map->GetTiles())
    {
        auto offset = utility::GetRandomPositionAround(OFFSET_RANGE);

        roadTile->Tile = &tile;
        roadTile->Offset = offset;

        roadTile++;
    }

    UpdateData();
}

void RoadModel::Render()
{
    UpdateData();

    static auto &pathSegments = WorldScene::Get()->GetPathSegments();

    static const auto camera = RenderManager::GetCamera(Cameras::WORLD);

    static auto shader = ShaderManager::GetShader("Road");
    shader->Bind();

    shader->SetConstant(camera->GetMatrix(), "viewMatrix");

	shader->SetConstant(1.0f, "opacity");

	shader->SetConstant(0.2f, "depth");

    buffer->Bind(0);

    /*rotationBuffer->Bind(1);

    thicknessBuffer->Bind(2);

    lengthBuffer->Bind(3);

    colorBuffer->Bind(4);*/

    glDrawArrays(GL_TRIANGLES, 0, 6 * pathSegments.GetSize());

    shader->Unbind();
}