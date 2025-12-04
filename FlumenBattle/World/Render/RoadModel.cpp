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

void RoadModel::Render()
{
    static const auto camera = RenderManager::GetCamera(Cameras::WORLD);

    static auto squareShader = ShaderManager::GetShader("Square");
    squareShader->Bind();

    squareShader->SetConstant(camera->GetMatrix(), "viewMatrix");

	squareShader->SetConstant(1.0f, "opacity");

	squareShader->SetConstant(0.2f, "depth");

    static auto &pathSegments = WorldScene::Get()->GetPathSegments();
    for(auto &segment : pathSegments)
    {
        auto tile = segment.To;
        auto nextTile = segment.From;

        auto position = (tile->Position + nextTile->Position) * 0.5f;
        squareShader->SetConstant(position, "hexPosition");

        Scale2 scale = Scale2(tile::WorldMap::WORLD_TILE_SIZE * 1.732f, 5.0f);
        squareShader->SetConstant(scale, "hexSize");

        auto color = [&segment] () 
        {
            return segment.Type == settlement::RoadTypes::UNTRODDEN ?
            Color(0.9f, 0.7f, 0.5f, 1.0f) * 0.6f :
            Color(0.7f, 0.7f, 0.7f, 1.0f) * 0.6f;
        } ();
        squareShader->SetConstant(color, "color");

        auto orientation = tile->Position - nextTile->Position;
        auto rotation = atan2(orientation.y, orientation.x);
        squareShader->SetConstant(rotation, "rotation");

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    squareShader->Unbind();
}