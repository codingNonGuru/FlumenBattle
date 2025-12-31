#include "FlumenEngine/Render/ShaderManager.hpp"
#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Render/Camera.hpp"
#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Interface/Sprite.hpp"
#include "FlumenEngine/Render/TextureManager.hpp"
#include "FlumenEngine/Render/DataBuffer.hpp"
#include "FlumenEngine/Core/Engine.hpp"
#include "FlumenEngine/Render/Screen.hpp"
#include "FlumenEngine/Render/BufferManager.hpp"
#include "FlumenEngine/Render/FrameBuffer.hpp"

#include "TerrainRenderer.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/WorldController.h"
#include "FlumenBattle/World/Tile/WorldMap.h"
#include "FlumenBattle/Types.hpp"

using namespace world::render;

static Camera *camera = nullptr;

static DataBuffer *positionBuffer = nullptr;

static DataBuffer *colorBuffer = nullptr;

static DataBuffer *elevationBuffer = nullptr;

static DataBuffer *heatBuffer = nullptr;

static DataBuffer *reliefBuffer = nullptr;

static DataBuffer *tileQueueBuffer = nullptr;

static container::Array <int> tileIndices;

TerrainRenderer::TerrainRenderer()
{
    distortMap = nullptr;
}

void TerrainRenderer::Initialize()
{
    camera = RenderManager::GetCamera(Cameras::WORLD);
    
    static const auto map = WorldScene::Get()->GetWorldMap();

    if(positionBuffer != nullptr)
        return;

    static auto positions = container::Array <Position2> (map->GetTileCount());

    static auto colors = container::Array <Color> (map->GetTileCount());

    static auto elevations = container::Array <Color> (map->GetTileCount());

    static auto temperatures = container::Array <Float> (map->GetTileCount());

    static auto reliefs = container::Array <int> (map->GetTileCount());

    tileIndices.Initialize(map->GetTileCount());

    for(auto &tile : map->GetTiles())
    //for(auto tile = map->GetTiles().GetStart(); tile != map->GetTiles().GetEnd(); ++tile)
    {
        *positions.Add() = tile.Position;

        //*colors.Add() = Color((float)tile->Elevation / 100.0f);
        *colors.Add() = [&]
        {
            //return Color::WHITE;
            static auto SEA_COLOR = Color(0.02f, 0.07f, 0.3f, 1.0f);

            if(tile.Elevation > 480)
                return Color::CYAN;
            else
            {
                auto factor = tile.Elevation / 48;
                float mixFactor = float(factor) / 10.0f;
                return Color::CYAN * mixFactor + SEA_COLOR * (1.0f - mixFactor);
            }
        } ();

        float factor = (float)tile.Elevation / 1000.0f;
        *elevations.Add() = Color(factor, factor, factor, 1.0f);

        *temperatures.Add() = tile.Type == WorldTiles::LAND ? (float)tile.Heat / (float)tile::WorldTile::MAXIMUM_TILE_HEAT : 1.0f;

        *reliefs.Add() = tile.Type == WorldTiles::LAND ? 1 : 0;
    }

    positionBuffer = new DataBuffer(positions.GetMemorySize(), positions.GetStart());

    colorBuffer = new DataBuffer(colors.GetMemorySize(), colors.GetStart());

    elevationBuffer = new DataBuffer(elevations.GetMemorySize(), elevations.GetStart());

    heatBuffer = new DataBuffer(temperatures.GetMemorySize(), temperatures.GetStart());

    reliefBuffer = new DataBuffer(reliefs.GetMemorySize(), reliefs.GetStart());

    tileQueueBuffer = new DataBuffer(tileIndices.GetMemoryCapacity(), tileIndices.GetStart());
}

void TerrainRenderer::Render()
{
    static const auto map = WorldScene::Get()->GetWorldMap();

    tileIndices.Reset();

    for(auto &tile : map->GetTiles())
    {
        if(tile.Type == WorldTiles::LAND)
        {
            auto index = &tile - map->GetTiles().GetStart();
            *tileIndices.Add() = index;
        }
    }

    tileQueueBuffer->UploadData(tileIndices.GetStart(), tileIndices.GetMemorySize());

    RenderLandTilesToDiffuseStencil();

    SharpenDiffuseStencil(Float3(0.95f, 0.85f, 0.7f));

    RenderSeaTilesToScreen();

    RenderLandTilesToScreen();

    tileIndices.Reset();

    for(auto &tile : map->GetTiles())
    {
        if(tile.Type == WorldTiles::LAND && (tile.HasBiome(world::WorldBiomes::STEPPE) || tile.HasBiome(world::WorldBiomes::WOODS)))
        {
            auto index = &tile - map->GetTiles().GetStart();
            *tileIndices.Add() = index;
        }
    }

    tileQueueBuffer->UploadData(tileIndices.GetStart(), tileIndices.GetMemorySize());

    RenderLandTilesToDiffuseStencil();

    SharpenDiffuseStencil(Float3(0.4f, 0.6f, 0.05f));

    RenderLandTilesToScreen();

    //RenderSteppeTilesToScreen();

    BufferManager::BindFrameBuffer(FrameBuffers::DEFAULT);
}

void TerrainRenderer::RenderLandTilesToDiffuseStencil()
{
    static auto stencilBuffer = BufferManager::GetFrameBuffer(FrameBuffers::STENCIL);
    stencilBuffer->BindBuffer();
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    stencilBuffer->Clear(Color::BLACK);

    RenderManager::EnableDepthTesting();
    RenderManager::SetBlendMode();

    static const auto map = WorldScene::Get()->GetWorldMap();

    static const auto tileShader = ShaderManager::GetShader("Tile");

    positionBuffer->Bind(0);

    reliefBuffer->Bind(1);

    tileQueueBuffer->Bind(2);

    tileShader->Bind();

    tileShader->SetConstant(camera->GetMatrix(), "viewMatrix");

    tileShader->SetConstant(1.0f, "opacity");

    tileShader->SetConstant(0.5f, "depth");

    tileShader->SetConstant(map->WORLD_TILE_SIZE * 5.0f, "hexSize");

    glDrawArrays(GL_TRIANGLES, 0, 6 * tileIndices.GetSize());

    tileShader->Unbind();
}

void TerrainRenderer::RenderSeaTilesToScreen()
{
    BufferManager::BindFrameBuffer(FrameBuffers::DEFAULT);

    static const auto map = WorldScene::Get()->GetWorldMap();

    auto newHexShader = ShaderManager::GetShader("BetterHex");

    newHexShader->Bind();

    newHexShader->SetConstant(camera->GetMatrix(), "viewMatrix");

	newHexShader->SetConstant(0.0f, "depth");

    newHexShader->SetConstant(1.0f, "opacity");

    newHexShader->SetConstant(map->WORLD_TILE_SIZE, "hexSize");

    positionBuffer->Bind(0);

    if(WorldController::Get()->GetDisplayMode() == 0)
        colorBuffer->Bind(1);
    else
        elevationBuffer->Bind(1);

    glDrawArrays(GL_TRIANGLES, 0, 18 * map->GetTileCount());

    newHexShader->Unbind();
}

void TerrainRenderer::SharpenDiffuseStencil(Float3 color)
{
    static auto finalStencilBuffer = BufferManager::GetFrameBuffer(FrameBuffers::FINAL_STENCIL);

    finalStencilBuffer->BindBuffer();
    finalStencilBuffer->Clear(Color{0.0f, 0.0f, 0.0f, 0.0f});

    static auto stencilBuffer = BufferManager::GetFrameBuffer(FrameBuffers::STENCIL);

    auto position = camera->GetTarget();

    static const auto shader = ShaderManager::GetShader("Sharpen");

    shader->Bind();

    shader->SetConstant(camera->GetMatrix(), "viewMatrix");

    shader->SetConstant(1.0f, "opacity");

    shader->SetConstant(0.1f, "depth");

    shader->SetConstant(position, "hexPosition");

    shader->SetConstant(Float2{1920.0f, 1080.0f} * camera->GetZoomFactor(), "hexSize");

    shader->SetConstant(color, "color");

    stencilBuffer->BindTexture(shader, "picture");

    shader->BindTexture(distortMap, "distort");

    glDrawArrays(GL_TRIANGLES, 0, 6);

    shader->Unbind();
}

void TerrainRenderer::RenderLandTilesToScreen()
{
    static auto finalStencilBuffer = BufferManager::GetFrameBuffer(FrameBuffers::FINAL_STENCIL);

    BufferManager::BindFrameBuffer(FrameBuffers::DEFAULT);

    static const auto blitShader = ShaderManager::GetShader("Blit");

    blitShader->Bind();

    blitShader->SetConstant(camera->GetMatrix(), "viewMatrix");

    blitShader->SetConstant(0.1f, "depth");

    auto position = camera->GetTarget();
    blitShader->SetConstant(position, "hexPosition");

    blitShader->SetConstant(Float2{1920.0f, 1080.0f} * camera->GetZoomFactor(), "hexSize");

    finalStencilBuffer->BindTexture(blitShader, "picture");

    glDrawArrays(GL_TRIANGLES, 0, 6);

    blitShader->Unbind();
}

void TerrainRenderer::RenderSteppeTilesToDiffuseStencil()
{

}

void TerrainRenderer::RenderSteppeTilesToScreen()
{

}