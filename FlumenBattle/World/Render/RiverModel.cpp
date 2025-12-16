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

#define RIVER_THICKNESS_FACTOR 0.25f

#define RIVER_BASE_THICKNESS 1.0f

#define SEGMENT_LENGTH 38.0f

#define SEGMENT_COUNT 6

#define COLOR_SHIFT_FACTOR 20.0f

#define RENDER_DEPTH 0.1f

#define OPACITY 0.8f

#define INDICES_PER_SEGMENT 6

#define SHADER_NAME "River"

#define BUFFER_BIND_POINT 0

void RiverModel::Initialize()
{
    auto map = WorldScene::Get()->GetWorldMap();

    for(auto &river : map->GetRivers())
    {
        for(auto &twist : river.GetTwists())
        {
            if(twist.First == nullptr || twist.Second == nullptr)
            {
                auto edge = twist.First != nullptr ? twist.First : twist.Second;
                auto edgePosition = edge->First->Position * 0.5f + edge->Second->Position * 0.5f;

                auto thickness = (float)edge->Discharge * RIVER_THICKNESS_FACTOR + RIVER_BASE_THICKNESS;

                float discharge = river.GetSegments().GetSize() - edge->Discharge;
                auto colorFactor = exp(-discharge * discharge / COLOR_SHIFT_FACTOR);

                *data.Add() = {{edgePosition, twist.Position * 0.5f + edgePosition * 0.5f, twist.Position}, {colorFactor, colorFactor}, {thickness, thickness}};
            }
            else
            {
                auto first = twist.First->First->Position * 0.5f + twist.First->Second->Position * 0.5f;
                auto second = twist.Second->First->Position * 0.5f + twist.Second->Second->Position * 0.5f;

                auto firstThickness = (float)twist.First->Discharge * RIVER_THICKNESS_FACTOR + RIVER_BASE_THICKNESS;
                auto secondThickness = (float)twist.Second->Discharge * RIVER_THICKNESS_FACTOR + RIVER_BASE_THICKNESS;

                float firstDischarge = river.GetSegments().GetSize() - twist.First->Discharge;
                float secondDischarge = river.GetSegments().GetSize() - twist.Second->Discharge;

                auto firstColor = exp(-firstDischarge * firstDischarge / COLOR_SHIFT_FACTOR);
                auto secondColor = exp(-secondDischarge * secondDischarge / COLOR_SHIFT_FACTOR);
                
                *data.Add() = {{first, twist.Position, second}, {firstColor, secondColor}, {firstThickness, secondThickness}};
            }
        }
    }

    buffer->UploadData(data.GetStart(), data.GetMemorySize());
}

void RiverModel::Render()
{
    static const auto camera = RenderManager::GetCamera(Cameras::WORLD);

    static auto shader = ShaderManager::GetShader(SHADER_NAME);
    shader->Bind();

    shader->SetConstant(camera->GetMatrix(), "viewMatrix");

	shader->SetConstant(OPACITY, "opacity");

	shader->SetConstant(RENDER_DEPTH, "depth");

    buffer->Bind(BUFFER_BIND_POINT);

    glDrawArrays(GL_TRIANGLES, 0, INDICES_PER_SEGMENT * SEGMENT_COUNT * data.GetSize());

    shader->Unbind();
}