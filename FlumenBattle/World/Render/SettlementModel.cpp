#include "FlumenEngine/Render/ShaderManager.hpp"
#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Render/Camera.hpp"
#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/DataBuffer.hpp"
#include "FlumenEngine/Core/Engine.hpp"

#include "SettlementModel.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Tile/WorldMap.h"
#include "FlumenBattle/World/Tile/WorldTile.h"
#include "FlumenBattle/World/Tile/WorldBiome.h"
#include "FlumenBattle/World/Render/WorldTileModel.h"
#include "FlumenBattle/World/Settlement/Settlement.h"

using namespace world;
using namespace world::render;

#define MAX_BUILDINGS_PER_SETTLEMENT 64

#define BASE_SCATTER_RANGE 10.0f

#define SCATTER_RANGE_INCREMENT 4.0f

#define MIN_DISTANCE_BETWEEN_BUILDINGS 12.0f

#define SIZE_RANGE 7.5f, 10.5f

#define RED_WEIGHT_RANGE 0.2f, 1.0f

#define BUILDINGS_PER_POP 3

#define FRUITLESS_SEARCH_LIMIT 5

#define INDICES_PER_BUILDING 6

#define BUILDING_SHADER_NAME "Building"

#define BUILDING_BUFFER_BIND_POINT 0

#define SETTLEMENT_INDEX_BIND_POINT 1

#define RENDER_OPACITY 1.0f

#define SHADOW_OPACITY 0.2f

#define RENDER_DEPTH 0.3f

#define SHADOW_RENDER_DEPTH 0.2f

static bool isRefreshedNeeded = true;

struct SettlementData
{
    const settlement::Settlement *Settlement;

    int Population;
};

container::Pool <SettlementData> populationTracker;

void SettlementModel::Initialize()
{
    static auto &scene = *WorldScene::Get();

    scene.OnSettlementFounded += []
    {
        *populationTracker.Add() = {WorldScene::Get()->GetFoundedSettlement(), WorldScene::Get()->GetFoundedSettlement()->GetPopulation()};

        isRefreshedNeeded = true;
    };

    auto map = scene.GetWorldMap();

    auto &settlements = scene.GetSettlements();

    buildingData.Initialize(settlements.GetCapacity() * MAX_BUILDINGS_PER_SETTLEMENT);

    for(auto& settlement : settlements)
    {
        container::SmartBlock <Position2, MAX_BUILDINGS_PER_SETTLEMENT> buildingPositions;

        for(int i = 0; i < MAX_BUILDINGS_PER_SETTLEMENT; ++i)
        {
            Position2 newPosition;
            auto scatterRange = BASE_SCATTER_RANGE;

            auto fruitlessSearchCount = 0;
            while(true)
            {
                if(fruitlessSearchCount == FRUITLESS_SEARCH_LIMIT)
                {
                    scatterRange += SCATTER_RANGE_INCREMENT;
                    fruitlessSearchCount = 0;
                }

                newPosition = utility::GetRandomPositionAround(scatterRange, settlement.GetLocation()->Position);

                bool isTooClose = false;
                for(auto &oldPosition : buildingPositions)
                {
                    if(glm::length(oldPosition - newPosition) < MIN_DISTANCE_BETWEEN_BUILDINGS)
                    {
                        isTooClose = true;
                        fruitlessSearchCount++;
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

            Float2 size = {utility::GetRandom(SIZE_RANGE), utility::GetRandom(SIZE_RANGE)};
            
            auto mixFactor = utility::GetRandom(RED_WEIGHT_RANGE);
            auto color = Color::RED * mixFactor + Color::YELLOW * (1.0f - mixFactor);

            color = Color::AddSaturation(color, utility::GetRandom(-0.6f, -0.1f));
            color = Color::Lighten(color, utility::GetRandom(-0.15f, 0.0f));

            *buildingData.Add() = {color, newPosition, size, rotation};
        }
    }

    settlementIndices.Initialize(settlements.GetCapacity());

    buildingDataBuffer = new DataBuffer(buildingData.GetMemoryCapacity(), buildingData.GetStart());

    indexBuffer = new DataBuffer(settlementIndices.GetMemoryCapacity(), settlementIndices.GetStart());

    populationTracker.Initialize(settlements.GetCapacity());

    for(auto& settlement : settlements)
    {
        *populationTracker.Add() = {&settlement, settlement.GetPopulation()};
    }

    Engine::OnLoopCycleStarted += Event{this, &SettlementModel::CheckPopulation};
}

void SettlementModel::CheckPopulation()
{
    for(auto &data : populationTracker)
    {
        auto newPopulation = data.Settlement->GetPopulation();
        if(newPopulation != data.Population)
        {
            isRefreshedNeeded = true;
            data.Population = newPopulation;
        }

        if(data.Settlement->IsCompletelyGone() == true)
        {
            populationTracker.RemoveAt(&data);
        }
    }
}

void SettlementModel::UpdateIndices()
{
    if(isRefreshedNeeded == false)
        return;

    settlementIndices.Reset();

    for(auto &settlement : WorldScene::Get()->GetSettlements())
    {
        auto popCount = settlement.GetPopulation();

        *settlementIndices.Add() = popCount * BUILDINGS_PER_POP;
    }

    indexBuffer->UploadData(settlementIndices.GetStart(), settlementIndices.GetMemorySize());

    isRefreshedNeeded = false;
}

void SettlementModel::Render()
{
    UpdateIndices();

    static const auto shader = ShaderManager::GetShader(BUILDING_SHADER_NAME);

    static const auto camera = RenderManager::GetCamera(Cameras::WORLD);

    shader->Bind();

    shader->SetConstant(camera->GetMatrix(), "viewMatrix");

	shader->SetConstant(RENDER_DEPTH, "depth");

    shader->SetConstant(RENDER_OPACITY, "opacity");

    shader->SetConstant(0, "mode");

    shader->SetConstant(MAX_BUILDINGS_PER_SETTLEMENT, "maxBuildings");

    buildingDataBuffer->Bind(BUILDING_BUFFER_BIND_POINT);

    indexBuffer->Bind(SETTLEMENT_INDEX_BIND_POINT);

    auto indexCount = INDICES_PER_BUILDING * MAX_BUILDINGS_PER_SETTLEMENT * settlementIndices.GetSize();
    glDrawArrays(GL_TRIANGLES, 0, indexCount);

    shader->Unbind();
}

void SettlementModel::RenderShadows()
{
    static const auto shader = ShaderManager::GetShader(BUILDING_SHADER_NAME);

    static const auto camera = RenderManager::GetCamera(Cameras::WORLD);

    shader->Bind();

    shader->SetConstant(camera->GetMatrix(), "viewMatrix");

	shader->SetConstant(SHADOW_RENDER_DEPTH, "depth");

    shader->SetConstant(SHADOW_OPACITY, "opacity");

    shader->SetConstant(1, "mode");

    shader->SetConstant(MAX_BUILDINGS_PER_SETTLEMENT, "maxBuildings");

    buildingDataBuffer->Bind(BUILDING_BUFFER_BIND_POINT);

    indexBuffer->Bind(SETTLEMENT_INDEX_BIND_POINT);

    auto indexCount = INDICES_PER_BUILDING * MAX_BUILDINGS_PER_SETTLEMENT * settlementIndices.GetSize();
    glDrawArrays(GL_TRIANGLES, 0, indexCount);

    shader->Unbind();
}