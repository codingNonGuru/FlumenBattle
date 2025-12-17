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

#define SCATTER_RANGE 35.0f

#define SIZE_RANGE 5.0f, 6.5f

#define INDICES_PER_BUILDING 6

#define BUILDING_SHADER_NAME "Shader"

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
        for(int i = 0; i < MAX_BUILDINGS_PER_SETTLEMENT; ++i)
        {
            auto position = utility::GetRandomPositionAround(SCATTER_RANGE, settlement.GetLocation()->Position);

            auto rotation = utility::GetRandom(0.0f, TWO_PI);

            auto size = utility::GetRandom(SIZE_RANGE);

            *buildingData.Add() = {Color::RED, position, rotation, size};
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