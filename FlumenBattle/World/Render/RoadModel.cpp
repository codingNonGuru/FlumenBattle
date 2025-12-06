#include "FlumenEngine/Render/ShaderManager.hpp"
#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Render/Camera.hpp"
#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/DataBuffer.hpp"

#include "RoadModel.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Tile/WorldMap.h"
#include "FlumenBattle/World/Tile/WorldTile.h"
#include "FlumenBattle/World/Settlement/Path.h"
#include "FlumenBattle/World/Settlement/Settlement.h"

using namespace world::render;

static bool doesDataNeedRefresh = true;

static bool doesQueueNeedRefresh = true;

void RoadModel::UpdateData()
{
    if(doesQueueNeedRefresh == false)
    {
        return;
    }

    doesQueueNeedRefresh = false;

    positions.Reset();

    colors.Reset();

    lengths.Reset();

    thicknesses.Reset();

    rotations.Reset();

    static auto &pathSegments = WorldScene::Get()->GetPathSegments();
    for(auto &segment : pathSegments)
    {
        auto tile = segment.To;
        auto nextTile = segment.From;

        auto position = (tile->Position + nextTile->Position) * 0.5f;
        *positions.Add() = position;

        auto length = tile::WorldMap::WORLD_TILE_SIZE * 1.732f;
        *lengths.Add() = length;

        *thicknesses.Add() = 5.0f;

        auto color = [&segment] () 
        {
            return segment.Type == settlement::RoadTypes::UNTRODDEN ?
            Color(0.9f, 0.7f, 0.5f, 1.0f) * 0.6f :
            Color(0.7f, 0.7f, 0.7f, 1.0f) * 0.6f;
        } ();
        *colors.Add() = color;

        auto orientation = tile->Position - nextTile->Position;
        auto rotation = atan2(orientation.y, orientation.x);
        *rotations.Add() = rotation;
    }

    positionBuffer->UploadData(positions.GetStart(), positions.GetMemoryCapacity());

    colorBuffer->UploadData(colors.GetStart(), colors.GetMemoryCapacity());

    lengthBuffer->UploadData(lengths.GetStart(), lengths.GetMemoryCapacity());

    thicknessBuffer->UploadData(thicknesses.GetStart(), thicknesses.GetMemoryCapacity());

    rotationBuffer->UploadData(rotations.GetStart(), rotations.GetMemoryCapacity());

    //tileQueueBuffer->UploadData(tileQueue.GetStart(), tileQueue.GetMemoryCapacity());
}

void RoadModel::Initialize()
{
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

    positionBuffer->Bind(0);

    rotationBuffer->Bind(1);

    thicknessBuffer->Bind(2);

    lengthBuffer->Bind(3);

    colorBuffer->Bind(4);

    glDrawArrays(GL_TRIANGLES, 0, 6 * pathSegments.GetSize());

    shader->Unbind();
}