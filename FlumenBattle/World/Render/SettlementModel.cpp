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

void SettlementModel::Initialize()
{

}

void SettlementModel::Render()
{
    static const auto shader = ShaderManager::GetShader("Hex");

    static const auto camera = RenderManager::GetCamera(Cameras::WORLD);

    static auto &scene = *WorldScene::Get();

    auto map = scene.GetWorldMap();

    shader->Bind();

    shader->SetConstant(camera->GetMatrix(), "viewMatrix");

	shader->SetConstant(0.3f, "depth");

    shader->SetConstant(1.0f, "opacity");

    auto settlements = scene.GetSettlements();
    for(auto& settlement : settlements)
    {
        if(settlement.IsValid() == false)
            continue;

        auto tile = settlement.GetLocation();

        shader->SetConstant(tile->Position, "hexPosition");

        auto size = [&settlement]
        {
            auto factor = pow((float)settlement.GetPopulation() + 1.0f, 0.5f);
            return factor * 0.15f * tile::WorldMap::WORLD_TILE_SIZE;
        } ();

        shader->SetConstant(size, "hexSize");

        shader->SetConstant(settlement.GetRulerBanner(), "color");
        //shader->SetConstant(settlement.GetBanner(), "color");

        glDrawArrays(GL_TRIANGLES, 0, 18);
    }

    shader->Unbind();
}