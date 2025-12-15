#include "FlumenEngine/Render/DataBuffer.hpp"
#include "FlumenEngine/Render/ShaderManager.hpp"
#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/Camera.hpp"
#include "FlumenEngine/Render/LineRenderer.h"

#include "RiverModel.h"
#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/World/Tile/WorldMap.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Tile/WorldTile.h"
#include "FlumenBattle/World/Tile/River.h"

using namespace world::render;

#define RIVER_THICKNESS_FACTOR 0.35f

#define SEGMENT_LENGTH 38.0f

#define SEGMENT_COUNT 6

void RiverModel::Initialize()
{
    auto map = WorldScene::Get()->GetWorldMap();

    data.Initialize(map->GetTileCount());

    for(auto &river : map->GetRivers())
    {
        for(auto &twist : river.GetTwists())
        {
            if(twist.First == nullptr || twist.Second == nullptr)
                continue;

            auto first = twist.First->First->Position * 0.5f + twist.First->Second->Position * 0.5f;
            auto second = twist.Second->First->Position * 0.5f + twist.Second->Second->Position * 0.5f;

            //auto thickness = (float)segment->Discharge * RIVER_THICKNESS_FACTOR;
            //thickness += 5.0f;

            *data.Add() = {Color::BLUE, {first, twist.Position, second}, 5.0f};
        }
    }

    buffer = new DataBuffer(data.GetMemoryCapacity(), data.GetStart());
}

void RiverModel::Render()
{
    static const auto camera = RenderManager::GetCamera(Cameras::WORLD);

    static auto shader = ShaderManager::GetShader("River");
    shader->Bind();

    shader->SetConstant(camera->GetMatrix(), "viewMatrix");

	shader->SetConstant(0.7f, "opacity");

	shader->SetConstant(0.1f, "depth");

    buffer->Bind(0);

    glDrawArrays(GL_TRIANGLES, 0, 6 * SEGMENT_COUNT * data.GetSize());

    shader->Unbind();
}