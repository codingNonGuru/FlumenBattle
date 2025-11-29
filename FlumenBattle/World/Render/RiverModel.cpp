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

using namespace world::render;

#define RIVER_THICKNESS_FACTOR 0.35f

void RiverModel::Render()
{
    static auto camera = RenderManager::GetCamera(Cameras::WORLD);

    auto map = WorldScene::Get()->GetWorldMap();

    for(auto &edge : map->GetEdges())
    {
        if(edge.River == nullptr)
            continue;

        auto position = edge.First->Position * 0.5f + edge.Second->Position * 0.5f;

        auto to = edge.First->Position - edge.Second->Position;
        auto rotation = atan2(to.y, to.x) + HALF_PI;

        auto thickness = (float)edge.Discharge * RIVER_THICKNESS_FACTOR;
        thickness += 5.0f;

        engine::render::LineRenderer::RenderLine(camera, position, 34.0f, thickness, rotation, Color::BLUE, 0.5f);
    }
}