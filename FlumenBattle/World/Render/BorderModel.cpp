#include "FlumenEngine/Render/DataBuffer.hpp"
#include "FlumenEngine/Render/ShaderManager.hpp"
#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/Camera.hpp"
#include "FlumenEngine/Core/Engine.hpp"
#include "FlumenEngine/Render/Screen.hpp"

#include "BorderModel.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/WorldMap.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/PreGame/PreGameState.h"

using namespace world::render;

static container::Array <Position2> positions;

static container::Array <float> rotations;

static container::Array <float> lengths;

static container::Array <float> thicknesses;

static container::Array <Float4> colors;

#define BUFFER_COUNT 5

static constexpr auto INTERNAL_BORDER_THICKNESS = 5.0f;

static constexpr auto EXTERNAL_BORDER_THICKNESS = 12.0f;

void BorderModel::Initialize()
{
    buffers_.Initialize(BUFFER_COUNT);

    static auto worldMap = WorldScene::Get()->GetWorldMap();
    
    positions.Initialize(worldMap->GetTileCount() * 3);

    rotations.Initialize(worldMap->GetTileCount() * 3);

    lengths.Initialize(worldMap->GetTileCount() * 3);

    thicknesses.Initialize(worldMap->GetTileCount() * 3);

    colors.Initialize(worldMap->GetTileCount() * 3);

    *buffers_.Add("Position") = new DataBuffer(positions.GetMemoryCapacity(), positions.GetStart());

    *buffers_.Add("Rotation") = new DataBuffer(rotations.GetMemoryCapacity(), rotations.GetStart());

    *buffers_.Add("Lengths") = new DataBuffer(lengths.GetMemoryCapacity(), lengths.GetStart());

    *buffers_.Add("Thickness") = new DataBuffer(thicknesses.GetMemoryCapacity(), thicknesses.GetStart());

    *buffers_.Add("Color") = new DataBuffer(colors.GetMemoryCapacity(), colors.GetStart());

    camera = RenderManager::GetCamera(Cameras::WORLD);

    tiles.Initialize(worldMap->GetTiles().GetWidth(), worldMap->GetTiles().GetHeight());

    auto worldTile = worldMap->GetTiles().GetStart();
    for(auto hexTile = tiles.GetTiles().GetStart(); hexTile != tiles.GetTiles().GetEnd(); ++hexTile, ++worldTile)
    {
        hexTile->Tile = worldTile;

        hexTile->Coordinates = worldTile->HexCoordinates;
    }

    for(auto hexTile = tiles.GetTiles().GetStart(); hexTile != tiles.GetTiles().GetEnd(); ++hexTile)
    {
        auto nearbyTiles = tiles.GetNearbyTiles(hexTile);

        auto index = 0;
        for(auto &nearbyTile : nearbyTiles)
        {
            *hexTile->Neighbours[index] = {nearbyTile, nullptr};

            index++;
        }
    }

    tiles.AddEdges();

    edgeValidators.Initialize(tiles.GetEdges().GetWidth(), tiles.GetEdges().GetHeight());
    for(auto validator = edgeValidators.GetStart(); validator != edgeValidators.GetEnd(); ++validator)
        *validator = false;

    for(auto hexTile = tiles.GetTiles().GetStart(); hexTile != tiles.GetTiles().GetEnd(); ++hexTile)
    {
        for(auto &neighbour : hexTile->Neighbours)
        {
            if(neighbour.Border == nullptr)
            {
                bool overflowsHorizontally = abs(hexTile->Tile->SquareCoordinates.x - neighbour.Neighbour->Tile->SquareCoordinates.x) > 1;
                if(overflowsHorizontally == true)
                    continue;

                bool overflowsVertically = abs(hexTile->Tile->SquareCoordinates.y - neighbour.Neighbour->Tile->SquareCoordinates.y) > 1;
                if(overflowsVertically == true)
                    continue;

                auto edgeCoordinates = tiles.GetEdgeCoordinates(hexTile, neighbour.Neighbour);

                auto border = tiles.GetEdges().Get(edgeCoordinates.x, edgeCoordinates.y);
                *border = {hexTile, neighbour.Neighbour};

                neighbour.Border = border;
                neighbour.Neighbour->AddBorder(hexTile, border);

                auto orientation = hexTile->Tile->Position - neighbour.Neighbour->Tile->Position;
                auto rotation = atan2(orientation.y, orientation.x);

                border->Rotation = rotation;
            }
        }
    }

    pregame::PreGameState::Get()->OnWorldGenerationFinished += {this, &BorderModel::HandleWorldGenerated};
}

Rectangle BorderModel::GetFrustum()
{
    static auto worldMap = WorldScene::Get()->GetWorldMap();

    const auto targetPosition =  camera->GetTarget();
    const auto targetTile = worldMap->GetTile(Float2{targetPosition.x, targetPosition.y});

    const auto coordinates = targetTile->SquareCoordinates;

    static const auto CAMERA_TO_WORLD_FACTOR = 9.0f;

    const auto zoomFactor = (0.0f + camera->GetZoomFactor()) * CAMERA_TO_WORLD_FACTOR;
    const auto screenRatio = Engine::GetScreen()->GetRatio();

    auto rectangle = Rectangle
    {
        {int((float)coordinates.x - zoomFactor * screenRatio), int((float)coordinates.y - zoomFactor)},
        {int((1.0f + zoomFactor) * 2.0f * screenRatio), int((1.0f + zoomFactor) * 2.0f)}    
    };

    utility::Clamp(rectangle.Position.x, 0, worldMap->GetTiles().GetWidth() - 1);
    utility::Clamp(rectangle.Position.y, 0, worldMap->GetTiles().GetHeight() - 1);

    utility::Clamp(rectangle.Size.x, 0, worldMap->GetTiles().GetWidth());
    utility::Clamp(rectangle.Size.y, 0, worldMap->GetTiles().GetHeight());

    return rectangle;
}

void BorderModel::HandleWorldGenerated()
{
    world::WorldScene::Get()->GetOwnershipChangeQueue();

    Update();
}

void BorderModel::Update()
{
    auto &changeQueue = world::WorldScene::Get()->GetOwnershipChangeQueue();

    for(auto &tile : changeQueue)
    {
        auto borderTile = tiles.GetTile(tile->HexCoordinates);

        for(auto &neighbour : borderTile->Neighbours)
        {
            if(neighbour.Border == nullptr)
                continue;
            
            auto border = neighbour.Border;

            auto edgeCoordinates = tiles.GetEdgeCoordinates(borderTile, neighbour.Neighbour);

            auto determineBorder = [border] (WorldTile *firstTile, WorldTile *secondTile, BorderData &borderData)
            {
                if(firstTile->IsOwned() == false)
                {
                    borderData.IsInitialized = false;
                    return false;
                }

                if(secondTile->IsOwned() == true && firstTile->GetOwner() == secondTile->GetOwner())
                {
                    borderData.IsInitialized = false;
                    return false;
                }

                bool isHardBorder = secondTile->IsOwned() == false || secondTile->GetOwner()->GetPolity() != firstTile->GetOwner()->GetPolity();

                auto position = (firstTile->Position + secondTile->Position) / 2.0f;
                if(isHardBorder == true)
                {
                    position = (position + firstTile->Position * 0.5f) / 1.5f;
                }
                
                auto length = WorldMap::WORLD_TILE_SIZE * 1.0f;
                auto thickness = isHardBorder == true ? EXTERNAL_BORDER_THICKNESS : INTERNAL_BORDER_THICKNESS;

                auto rotation = border->Rotation;
                
                auto color = firstTile->GetOwner()->GetRulerBanner();

                borderData = {position, rotation, thickness, length, color, true};

                return true;
            };

            auto firstHasNoBorder = determineBorder(tile, neighbour.Neighbour->Tile, border->FirstData);
            auto secondHasNoBorder = determineBorder(neighbour.Neighbour->Tile, tile, border->SecondData);
            if(firstHasNoBorder == false && secondHasNoBorder == false)
            {
                *edgeValidators.Get(edgeCoordinates.x, edgeCoordinates.y) = false;
            }
            else
            {
                *edgeValidators.Get(edgeCoordinates.x, edgeCoordinates.y) = true;
            }
        }
    }
}

void BorderModel::TransferData()
{
    auto frustum = GetFrustum();

    positions.Reset();

    rotations.Reset();

    lengths.Reset();

    thicknesses.Reset();

    colors.Reset();

    auto index = 0;
    for(auto y = frustum.Position.y; y < frustum.Position.y + frustum.Size.y; ++y)
    {
        std::pair <int, int> ranges[2] = {
            {frustum.Position.x, frustum.Position.x + frustum.Size.x}, 
            {tiles.GetTiles().GetWidth() + frustum.Position.x * 2, tiles.GetTiles().GetWidth() + frustum.Position.x * 2 + frustum.Size.x * 2}
            };

        for(auto range : ranges)
        {
            for(auto x = range.first; x < range.second; ++x)
            {
                index++;

                if(*edgeValidators.Get(x, y) == false)
                    continue;

                auto border = tiles.GetEdges().Get(x, y);

                auto fetchData = [] (BorderData &data)
                {
                    if(data.IsInitialized == false)
                        return;

                    *positions.Add() = data.Position;

                    *rotations.Add() = data.Rotation;

                    *lengths.Add() = data.Length;

                    *thicknesses.Add() = data.Thickness;

                    *colors.Add() = data.Color;
                };

                fetchData(border->FirstData);
                fetchData(border->SecondData);
            }
        }
    }

    (*buffers_.Get("Position"))->UploadData(positions.GetStart(), positions.GetMemorySize());

    (*buffers_.Get("Rotation"))->UploadData(rotations.GetStart(), rotations.GetMemorySize());

    (*buffers_.Get("Lengths"))->UploadData(lengths.GetStart(), lengths.GetMemorySize());

    (*buffers_.Get("Thickness"))->UploadData(thicknesses.GetStart(), thicknesses.GetMemorySize());

    (*buffers_.Get("Color"))->UploadData(colors.GetStart(), colors.GetMemorySize());

    std::cout<<index<<"\n";
}

void BorderModel::Render()
{
    Update(); 

    TransferData();

    static auto lineShader = ShaderManager::GetShader("Line");

    lineShader->Bind();

    lineShader->SetConstant(camera->GetMatrix(), "viewMatrix");

	lineShader->SetConstant(0.5f, "opacity");

	lineShader->SetConstant(0.0f, "depth");

    (*buffers_.Get("Position"))->Bind(0);

    (*buffers_.Get("Rotation"))->Bind(1);

    (*buffers_.Get("Lengths"))->Bind(2);

    (*buffers_.Get("Thickness"))->Bind(3);

    (*buffers_.Get("Color"))->Bind(4);

    static const auto INDICES_PER_LINE = 6;
    glDrawArrays(GL_TRIANGLES, 0, INDICES_PER_LINE * positions.GetSize());

    lineShader->Unbind();

    /*auto map = worldScene->GetWorldMap();
    for(auto tile = map->tiles.GetStart(); tile != map->tiles.GetEnd(); ++tile)
    {
        if(tile->IsOwned() == false)
            continue;

        auto nearbyTiles = tile->GetNearbyTiles();
        for(auto &neighbour : nearbyTiles)
        {
            if(neighbour->IsOwned() == true && tile->GetOwner() == neighbour->GetOwner())
                continue;

            bool isHardBorder = neighbour->IsOwned() == false || neighbour->GetOwner()->GetPolity() != tile->GetOwner()->GetPolity();

            auto position = (tile->Position + neighbour->Position) / 2.0f;
            if(isHardBorder == true)
            {
                position = (position + tile->Position * 0.5f) / 1.5f;
            }
            squareShader->SetConstant(position, "hexPosition");
        
            Scale2 scale = Scale2(WORLD_TILE_SIZE * 1.0f, isHardBorder ? 20.0f : 3.0f);
            squareShader->SetConstant(scale, "hexSize");

            auto orientation = tile->Position - neighbour->Position;
            auto rotation = atan2(orientation.y, orientation.x) + 1.5707f;
            squareShader->SetConstant(rotation, "rotation");

            squareShader->SetConstant(tile->GetOwner()->GetRulerBanner(), "color");

            glDrawArrays(GL_TRIANGLES, 0, 6);

            if(isHardBorder)
            {
                for(auto &secondaryNeighbour : nearbyTiles)
                {
                    if(secondaryNeighbour->GetDistanceTo(*neighbour) != 1)
                        continue;

                    if(secondaryNeighbour->IsOwned() == false)
                        continue;

                    if(neighbour->IsOwned() && secondaryNeighbour->GetOwner()->GetPolity() == neighbour->GetOwner()->GetPolity())
                        continue;

                    if(tile->GetOwner()->GetPolity() != secondaryNeighbour->GetOwner()->GetPolity())
                        continue;

                    auto position = (tile->Position + neighbour->Position + secondaryNeighbour->Position) / 3.0f;
                    position = (position + tile->Position * 0.36f + secondaryNeighbour->Position * 0.36f) / 1.72f;
                    squareShader->SetConstant(position, "hexPosition");
                
                    Scale2 scale = Scale2(WORLD_TILE_SIZE * 0.57f, 20.0f);
                    squareShader->SetConstant(scale, "hexSize");

                    auto orientation = tile->Position - secondaryNeighbour->Position;
                    auto rotation = atan2(orientation.y, orientation.x);
                    squareShader->SetConstant(rotation, "rotation");

                    squareShader->SetConstant(tile->GetOwner()->GetRulerBanner(), "color");

                    glDrawArrays(GL_TRIANGLES, 0, 6);
                }
            }
        }
    }*/
}