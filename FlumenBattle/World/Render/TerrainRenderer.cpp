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
#include "FlumenEngine/Render/Texture.hpp"
#include "FlumenEngine/Render/ImageProcessor.hpp"
#include "FlumenEngine/Utility/Kernel.hpp"

#include "TerrainRenderer.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/WorldController.h"
#include "FlumenBattle/World/Tile/WorldMap.h"
#include "FlumenBattle/Types.hpp"

using namespace world::render;

static Camera *camera = nullptr;

static DataBuffer *positionBuffer = nullptr;

static DataBuffer *cornerPositionBuffer = nullptr;

static DataBuffer *cornerColorBuffer = nullptr;

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

    static auto cornerPositions = container::Array <Position2> (map->GetCorners().GetObjectCount());

    static auto colors = container::Array <Color> (map->GetTileCount());

    static auto cornerColors = container::Array <Color> (map->GetCorners().GetObjectCount());

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


    for(auto &corner : map->GetCorners())
    {
        *cornerPositions.Add() = corner.Position;

        *cornerColors.Add() = Color(1.0f, 0.0f, 0.0f, 1.0f);
    }

    cornerPositionBuffer = new DataBuffer(cornerPositions.GetMemorySize(), cornerPositions.GetStart());

    cornerColorBuffer = new DataBuffer(cornerColors.GetMemorySize(), cornerColors.GetStart());
}

void TerrainRenderer::Render()
{
    //ClearStencilBuffer(Color::BLACK);

    RenderSeaTilesToScreen();

    static auto stencilBuffer = BufferManager::GetFrameBuffer(FrameBuffers::STENCIL);

    static auto colorTexture = stencilBuffer->GetColorTexture();

    static auto blurredTexture = new ::render::Texture(Size(1920, 1080), TextureFormats::FOUR_FLOAT);

    static auto blurKernel = ImageProcessor::GenerateKernel(16);

    //ImageProcessor::Blur(colorTexture, blurredTexture, Size(1920, 1080), blurKernel);

    //BufferManager::BindFrameBuffer(FrameBuffers::DEFAULT);

    //RenderTextureToScreen(colorTexture, 0.01f);

    static const auto map = WorldScene::Get()->GetWorldMap();

    auto const DESERT_COLOR = Color(0.9f, 0.4f, 0.2f, 1.0f);
    auto const DIRT_COLOR = Color(0.9f, 0.7f, 0.5f, 1.0f);
    auto const GRASS_COLOR = Color(0.4f, 0.6f, 0.05f, 1.0f);


    BufferManager::ClearColor(FrameBuffers::STENCIL, Color::BLACK);

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

    RenderHexesToDiffuseStencil(positionBuffer, Color::WHITE, 4.5f);


    tileIndices.Reset();

    for(auto &corner : map->GetCorners())
    {
        if(corner.IsFullyLand == true)
        {
            auto index = &corner - map->GetCorners().GetStart();
            *tileIndices.Add() = index;
        }
    }

    tileQueueBuffer->UploadData(tileIndices.GetStart(), tileIndices.GetMemorySize());

    RenderHexesToDiffuseStencil(cornerPositionBuffer, Color::WHITE, 5.5f);


    SharpenDiffuseStencil(BufferManager::GetFrameBuffer(FrameBuffers::STENCIL), BufferManager::GetFrameBuffer(FrameBuffers::TERRAIN_BASE_STENCIL), DESERT_COLOR, Range(0.499f, 1.1f), 0.4f);

    RenderTextureToScreen(BufferManager::GetFrameBuffer(FrameBuffers::TERRAIN_BASE_STENCIL)->GetColorTexture(), 0.02f);


    
    
    BufferManager::ClearColor(FrameBuffers::STENCIL_2, Color::BLACK);
    
    tileIndices.Reset();
    
    for(auto &tile : map->GetTiles())
    {
        if(tile.Type == WorldTiles::LAND && tile.HasBiome(WorldBiomes::DESERT) == false)
        {
            auto index = &tile - map->GetTiles().GetStart();
            *tileIndices.Add() = index;
        }
    }
    
    tileQueueBuffer->UploadData(tileIndices.GetStart(), tileIndices.GetMemorySize());
    
    RenderHexesToDiffuseStencil(positionBuffer, Color::WHITE, 4.5f);
    
    
    tileIndices.Reset();
    
    for(auto &corner : map->GetCorners())
    {
        if(corner.IsFullySteppe == true)
        {
            auto index = &corner - map->GetCorners().GetStart();
            *tileIndices.Add() = index;
        }
    }
    
    tileQueueBuffer->UploadData(tileIndices.GetStart(), tileIndices.GetMemorySize());
    
    RenderHexesToDiffuseStencil(cornerPositionBuffer, Color::WHITE, 5.5f);
    
    
    SharpenDiffuseStencil(BufferManager::GetFrameBuffer(FrameBuffers::STENCIL_2), BufferManager::GetFrameBuffer(FrameBuffers::FINAL_STENCIL), GRASS_COLOR, Range(0.499f, 1.1f), 0.4f);
    
    RenderTextureToScreen(BufferManager::GetFrameBuffer(FrameBuffers::FINAL_STENCIL)->GetColorTexture(), 0.025f);
    


    
    static const auto BEACH_COLOR = DIRT_COLOR * 0.5f + Color::WHITE * 0.5f;
    SharpenDiffuseStencil(BufferManager::GetFrameBuffer(FrameBuffers::STENCIL), BufferManager::GetFrameBuffer(FrameBuffers::TERRAIN_BASE_STENCIL), BEACH_COLOR, Range(0.499f, 0.6f), 0.4f);

    RenderTextureToScreen(BufferManager::GetFrameBuffer(FrameBuffers::TERRAIN_BASE_STENCIL)->GetColorTexture(), 0.03f);


    SharpenDiffuseStencil(BufferManager::GetFrameBuffer(FrameBuffers::STENCIL), BufferManager::GetFrameBuffer(FrameBuffers::TERRAIN_BASE_STENCIL), Color::WHITE, Range(0.35f, 0.4f), 0.25f);

    RenderTextureToScreen(BufferManager::GetFrameBuffer(FrameBuffers::TERRAIN_BASE_STENCIL)->GetColorTexture(), 0.04f);

    SharpenDiffuseStencil(BufferManager::GetFrameBuffer(FrameBuffers::STENCIL), BufferManager::GetFrameBuffer(FrameBuffers::TERRAIN_BASE_STENCIL), Color::WHITE, Range(0.26f, 0.29f), 0.25f);

    RenderTextureToScreen(BufferManager::GetFrameBuffer(FrameBuffers::TERRAIN_BASE_STENCIL)->GetColorTexture(), 0.04f);
    
    /*tileIndices.Reset();
    
    for(auto &tile : map->GetTiles())
    {
        if(tile.Type == WorldTiles::LAND && (tile.IsArid() == false))
        {
            auto index = &tile - map->GetTiles().GetStart();
            *tileIndices.Add() = index;
        }
    }

    tileQueueBuffer->UploadData(tileIndices.GetStart(), tileIndices.GetMemorySize());

    RenderLandTilesToDiffuseStencil();

    SharpenDiffuseStencil(DIRT_COLOR * 0.9f + Color::WHITE * 0.1f);

    RenderLandTilesToScreen(0.06f);


    tileIndices.Reset();

    for(auto &tile : map->GetTiles())
    {
        if(tile.Type == WorldTiles::LAND && tile.HasBiome(WorldBiomes::DESERT) == false)
        {
            auto index = &tile - map->GetTiles().GetStart();
            *tileIndices.Add() = index;
        }
    }

    tileQueueBuffer->UploadData(tileIndices.GetStart(), tileIndices.GetMemorySize());

    RenderLandTilesToDiffuseStencil();

    SharpenDiffuseStencil(GRASS_COLOR * 0.7f + DIRT_COLOR * 0.3f);

    RenderLandTilesToScreen(0.07f);


    tileIndices.Reset();

    for(auto &tile : map->GetTiles())
    {
        if(tile.Type == WorldTiles::LAND && tile.HasBiome(WorldBiomes::STEPPE) == true && tile.IsScrubland == true)
        {
            auto index = &tile - map->GetTiles().GetStart();
            *tileIndices.Add() = index;
        }
    }

    tileQueueBuffer->UploadData(tileIndices.GetStart(), tileIndices.GetMemorySize());

    RenderLandTilesToDiffuseStencil();

    SharpenDiffuseStencil(GRASS_COLOR * 0.3f + DIRT_COLOR * 0.7f);

    RenderLandTilesToScreen(0.08f);*/


    BufferManager::BindFrameBuffer(FrameBuffers::DEFAULT);
}

void TerrainRenderer::ClearStencilBuffer(Color color)
{
    static auto stencilBuffer = BufferManager::GetFrameBuffer(FrameBuffers::STENCIL);
    stencilBuffer->BindBuffer();
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    stencilBuffer->Clear(color);
}

void TerrainRenderer::RenderHexesToDiffuseStencil(DataBuffer *mainBuffer, Color color, float sizeFactor)
{
    static const auto map = WorldScene::Get()->GetWorldMap();

    static const auto tileShader = ShaderManager::GetShader("Tile");

    RenderManager::EnableDepthTesting();
    RenderManager::SetBlendMode();

    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    //glEnable(GL_MULTISAMPLE);
    //glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);

    mainBuffer->Bind(0);
    reliefBuffer->Bind(1);
    tileQueueBuffer->Bind(2);

    tileShader->Bind();

    tileShader->SetConstant(camera->GetMatrix(), "viewMatrix");
    tileShader->SetConstant(1.0f, "opacity");
    tileShader->SetConstant(0.5f, "depth");
    tileShader->SetConstant(color, "color");
    tileShader->SetConstant(map->WORLD_TILE_SIZE * sizeFactor, "hexSize");

    static const auto blobTexture = ::render::TextureManager::GetTexture("GaussianBlot");

    tileShader->BindTexture(blobTexture, "blob");

    glDrawArrays(GL_TRIANGLES, 0, 6 * tileIndices.GetSize());

    tileShader->Unbind();

    //RenderManager::EnableDepthTesting();
}

void TerrainRenderer::RenderCornersToScreen()
{
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);

    auto hoveredTile = WorldController::Get()->GetHoveredTile();
    if(hoveredTile == nullptr)
        return;

    static const auto map = WorldScene::Get()->GetWorldMap();

    auto newHexShader = ShaderManager::GetShader("BetterHex");

    newHexShader->Bind();

    newHexShader->SetConstant(camera->GetMatrix(), "viewMatrix");

	newHexShader->SetConstant(0.5f, "depth");

    newHexShader->SetConstant(1.0f, "opacity");

    newHexShader->SetConstant(map->WORLD_TILE_SIZE * 0.2f, "hexSize");

    //cornerPositionBuffer->Bind(0);

    auto corners = map->GetCornersOfHex(hoveredTile);

    static auto positions = container::Array <Position2> (6);

    positions.Reset();

    for(auto &corner : corners)
    {
        *positions.Add() = corner->Position;
    }

    cornerPositionBuffer->UploadData(positions.GetStart(), positions.GetMemorySize());

    cornerPositionBuffer->Bind(0);

    cornerColorBuffer->Bind(1);

    //glDrawArrays(GL_TRIANGLES, 0, 18 * map->GetCorners().GetObjectCount());
    glDrawArrays(GL_TRIANGLES, 0, 18 * 6);

    /*auto corner = map->GetCorner(100, 100);

    static auto positions = container::Array <Position2> (4);

    positions.Reset();

    *positions.Add() = corner->Position;
    *positions.Add() = corner->First->Position;
    *positions.Add() = corner->Second->Position;
    *positions.Add() = corner->Third->Position;

    cornerPositionBuffer->UploadData(positions.GetStart(), positions.GetMemorySize());

    cornerPositionBuffer->Bind(0);

    cornerColorBuffer->Bind(1);

    glDrawArrays(GL_TRIANGLES, 0, 18 * 4);*/

    newHexShader->Unbind();

    glDisable(GL_MULTISAMPLE);
}

void TerrainRenderer::RenderSeaTilesToScreen()
{
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);

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

    glDisable(GL_MULTISAMPLE);
}

void TerrainRenderer::SharpenDiffuseStencil(FrameBuffer *sourceBuffer, FrameBuffer *targetBuffer, Color color, Range heightRange, float distortFactor)
{
    //static auto finalStencilBuffer = BufferManager::GetFrameBuffer(FrameBuffers::FINAL_STENCIL);

    targetBuffer->BindBuffer();
    targetBuffer->Clear(Color{0.0f, 0.0f, 0.0f, 0.0f});

    //static auto stencilBuffer = BufferManager::GetFrameBuffer(FrameBuffers::STENCIL);

    auto position = camera->GetTarget();

    static const auto shader = ShaderManager::GetShader("Sharpen");

    shader->Bind();

    shader->SetConstant(camera->GetMatrix(), "viewMatrix");

    shader->SetConstant(1.0f, "opacity");

    shader->SetConstant(0.1f, "depth");

    shader->SetConstant(position, "hexPosition");

    shader->SetConstant(Float2{1920.0f, 1080.0f} * camera->GetZoomFactor(), "hexSize");

    shader->SetConstant(color.GetRGB(), "color");

    shader->SetConstant(heightRange, "heightRange");

    shader->SetConstant(distortFactor, "distortFactor");

    sourceBuffer->BindTexture(shader, "picture");

    shader->BindTexture(distortMap, "distort");

    glDrawArrays(GL_TRIANGLES, 0, 6);

    shader->Unbind();
}

void TerrainRenderer::RenderTextureToScreen(::render::Texture *texture, float depth)
{
    BufferManager::BindFrameBuffer(FrameBuffers::DEFAULT);

    static const auto blitShader = ShaderManager::GetShader("Blit");

    blitShader->Bind();

    blitShader->SetConstant(camera->GetMatrix(), "viewMatrix");

    blitShader->SetConstant(depth, "depth");

    auto position = camera->GetTarget();
    blitShader->SetConstant(position, "hexPosition");

    blitShader->SetConstant(Float2{1920.0f, 1080.0f} * camera->GetZoomFactor(), "hexSize");

    blitShader->BindTexture(texture, "picture");

    glDrawArrays(GL_TRIANGLES, 0, 6);

    blitShader->Unbind();
}

void TerrainRenderer::RenderSteppeTilesToDiffuseStencil()
{

}