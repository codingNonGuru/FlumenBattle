#include "FlumenEngine/Render/ShaderManager.hpp"
#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Render/Camera.hpp"
#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/DataBuffer.hpp"

#include "SettlementModel.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Tile/WorldMap.h"
#include "FlumenBattle/World/Tile/WorldTile.h"
#include "FlumenBattle/World/Tile/WorldBiome.h"
#include "FlumenBattle/World/Render/WorldTileModel.h"
#include "FlumenBattle/World/Settlement/Settlement.h"

using namespace world::render;

#define MAX_BUILDINGS_PER_SETTLEMENT 64

#define SCATTER_RANGE 50.0f

#define MIN_DISTANCE_BETWEEN_BUILDINGS 9.0f

#define SIZE_RANGE 7.0f, 9.0f

#define INDICES_PER_BUILDING 6

#define BUILDING_SHADER_NAME "Building"

#define BUILDING_BUFFER_BIND_POINT 0

#define RENDER_OPACITY 1.0f

#define RENDER_DEPTH 0.3f

void SettlementModel::Initialize()
{
    static auto &scene = *WorldScene::Get();

    auto map = scene.GetWorldMap();

    auto &settlements = scene.GetSettlements();

    buildingData.Initialize(settlements.GetCapacity() * MAX_BUILDINGS_PER_SETTLEMENT);

    for(auto& settlement : settlements)
    {
        container::SmartBlock <Position2, MAX_BUILDINGS_PER_SETTLEMENT> buildingPositions;

        for(int i = 0; i < MAX_BUILDINGS_PER_SETTLEMENT; ++i)
        {
            Position2 newPosition;
            while(true)
            {
                newPosition = utility::GetRandomPositionAround(SCATTER_RANGE, settlement.GetLocation()->Position);

                bool isTooClose = false;
                for(auto &oldPosition : buildingPositions)
                {
                    if(glm::length(oldPosition - newPosition) < MIN_DISTANCE_BETWEEN_BUILDINGS)
                    {
                        isTooClose = true;
                        break;
                    }
                }

                if(isTooClose == false)
                {
                    *buildingPositions.Add() = newPosition;
                    break;
                }
            }

            auto rotation = utility::GetRandom(0.0f, TWO_PI);

            Float2 size = {utility::GetRandom(SIZE_RANGE), utility::GetRandom(SIZE_RANGE)};\
            
            auto mixFactor = utility::GetRandom(0.3f, 1.0f);
            auto color = Color::RED * mixFactor + Color::YELLOW * (1.0f - mixFactor);

            color = Color::AddSaturation(color, utility::GetRandom(-0.5f, -0.1f));
            color = Color::Lighten(color, utility::GetRandom(-0.05f, 0.05f));

            *buildingData.Add() = {color, newPosition, size, rotation};
        }
    }

    buildingDataBuffer = new DataBuffer(buildingData.GetMemoryCapacity(), buildingData.GetStart());
}

void SettlementModel::Render()
{
    static const auto shader = ShaderManager::GetShader(BUILDING_SHADER_NAME);

    static const auto camera = RenderManager::GetCamera(Cameras::WORLD);

    shader->Bind();

    shader->SetConstant(camera->GetMatrix(), "viewMatrix");

	shader->SetConstant(RENDER_DEPTH, "depth");

    shader->SetConstant(RENDER_OPACITY, "opacity");

    buildingDataBuffer->Bind(BUILDING_BUFFER_BIND_POINT);

    glDrawArrays(GL_TRIANGLES, 0, INDICES_PER_BUILDING * buildingData.GetSize());

    shader->Unbind();
}