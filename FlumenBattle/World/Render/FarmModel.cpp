#include "FlumenEngine/Render/DataBuffer.hpp"
#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Render/ShaderManager.hpp"
#include "FlumenEngine/Interface/Sprite.hpp"
#include "FlumenEngine/Render/Camera.hpp"
#include "FlumenEngine/Render/RenderManager.hpp"

#include "FarmModel.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/SettlementTile.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/WorldMap.h"
#include "FlumenBattle/World/Render/WorldTileModel.h"

using namespace world::render;

#define BUFFER_COUNT 3

static constexpr auto POSITION_BUFFER_NAME = "Position";

static constexpr auto OFFSET_BUFFER_NAME = "TextureOffset";

static constexpr auto ROTATION_BUFFER_NAME = "Rotation";

static constexpr auto SPRITE_SIZE = 0.26f;

static constexpr auto TEXTURE_CROP_SIZE = Scale2(0.25f, 1.0f);

void FarmModel::Initialize()
{
    camera = RenderManager::GetCamera(Cameras::WORLD);

    buffers_.Initialize(BUFFER_COUNT);

    *buffers_.Add(POSITION_BUFFER_NAME) = new DataBuffer(positions.GetMemoryCapacity(), positions.GetStart());

    *buffers_.Add(OFFSET_BUFFER_NAME) = new DataBuffer(textureOffsets.GetMemoryCapacity(), textureOffsets.GetStart());

    *buffers_.Add(ROTATION_BUFFER_NAME) = new DataBuffer(rotations.GetMemoryCapacity(), rotations.GetStart());

    for(auto data = farmData.GetStart(); data != farmData.GetEnd(); ++data)
    {
        *data = {utility::GetRandom(0, 2), utility::GetRandom(0.0f, TWO_PI)};
    }
}

void FarmModel::TransferData()
{
    positions.Reset();

    textureOffsets.Reset();
    
    rotations.Reset();

    const auto frustum = WorldTileModel::Get()->GetFrustum();

    auto &settlements = WorldScene::Get()->GetSettlements();

    for(auto &settlement : settlements)
    {
        if(frustum.IsInside(settlement.GetLocation()->SquareCoordinates) == false)
            continue;
         
        for(auto tile : settlement.GetTiles())
        {
            if(tile.IsImproved() == false)
                continue;

            if(tile.HasImprovement(settlement::TileImprovements::FARM) == false)
                continue;

            *positions.Add() = tile.Tile->Position;

            auto data = farmData.Get(tile.Tile->SquareCoordinates.x, tile.Tile->SquareCoordinates.y);

            if(data->Index == 0)
                *textureOffsets.Add() = Scale2(0.0f, 0.0f);
            else if(data->Index == 1)
                *textureOffsets.Add() = Scale2(0.25f, 0.0f);
            else if(data->Index == 2)
                *textureOffsets.Add() = Scale2(0.5f, 0.0f);

            *rotations.Add() = data->Rotation;
        }
    }

    (*buffers_.Get(POSITION_BUFFER_NAME))->UploadData(positions.GetStart(), positions.GetMemorySize());

    (*buffers_.Get(OFFSET_BUFFER_NAME))->UploadData(textureOffsets.GetStart(), textureOffsets.GetMemorySize());

    (*buffers_.Get(ROTATION_BUFFER_NAME))->UploadData(rotations.GetStart(), rotations.GetMemorySize());
}

void FarmModel::Render()
{
    TransferData();

    static const auto massShader = ShaderManager::GetShader("ComplexMassSprite");

    static const auto sprite = new Sprite(massShader, "Farm");

    massShader->Bind();

    massShader->SetConstant(camera->GetMatrix(), "viewMatrix");

	massShader->SetConstant(0.0f, "depth");

    massShader->SetConstant(TEXTURE_CROP_SIZE, "textureScale");

    massShader->SetConstant(SPRITE_SIZE, "spriteSize");

    massShader->SetConstant(1, "hasRotation");

    massShader->SetConstant(0, "hasOpacity");

    massShader->SetConstant(0, "hasFlip");

    (*buffers_.Get(POSITION_BUFFER_NAME))->Bind(0);

    (*buffers_.Get(OFFSET_BUFFER_NAME))->Bind(1);

    (*buffers_.Get(ROTATION_BUFFER_NAME))->Bind(4);

    sprite->BindDefaultTextures();

    glDrawArrays(GL_TRIANGLES, 0, 6 * positions.GetSize());
}