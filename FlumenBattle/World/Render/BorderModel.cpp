#include "FlumenEngine/Render/DataBuffer.hpp"
#include "FlumenEngine/Render/ShaderManager.hpp"
#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/Camera.hpp"

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

void BorderModel::Initialize()
{
    buffers_.Initialize(5);

    static auto worldMap = WorldScene::Get()->GetWorldMap();
    
    positions.Initialize(worldMap->GetTileCount() * 10);

    rotations.Initialize(worldMap->GetTileCount() * 10);

    lengths.Initialize(worldMap->GetTileCount() * 10);

    thicknesses.Initialize(worldMap->GetTileCount() * 10);

    colors.Initialize(worldMap->GetTileCount() * 10);

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

    /*for(auto hexTile = tiles.GetTiles().GetStart(); hexTile != tiles.GetTiles().GetEnd(); ++hexTile)
    {
        for(auto &neighbour : hexTile->Neighbours)
        {
            if(neighbour.Border == nullptr)
            {
                auto newBorder = borders.Add();
                *newBorder = {hexTile, neighbour.Neighbour};

                neighbour.Border = newBorder;
                neighbour.Neighbour->AddBorder(hexTile, newBorder);

                auto orientation = hexTile->Tile->Position - neighbour.Neighbour->Tile->Position;
                auto rotation = atan2(orientation.y, orientation.x);

                newBorder->Rotation = rotation;
            }
        }
    }*/

    for(auto hexTile = tiles.GetTiles().GetStart(); hexTile != tiles.GetTiles().GetEnd(); ++hexTile)
    {
        for(auto &neighbour : hexTile->Neighbours)
        {
            if(neighbour.Border == nullptr)
            {
                if(abs(hexTile->Tile->SquareCoordinates.x - neighbour.Neighbour->Tile->SquareCoordinates.x) > 1)
                    continue;

                if(abs(hexTile->Tile->SquareCoordinates.y - neighbour.Neighbour->Tile->SquareCoordinates.y) > 1)
                    continue;

                //std::cout<<hexTile->Tile->SquareCoordinates.x<<" "<<hexTile->Tile->SquareCoordinates.y<<" -- ";
                //std::cout<<neighbour.Neighbour->Tile->SquareCoordinates.x<<" "<<neighbour.Neighbour->Tile->SquareCoordinates.y<<" -------- ";

                auto border = tiles.GetEdge(hexTile, neighbour.Neighbour);
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

void BorderModel::HandleWorldGenerated()
{
    world::WorldScene::Get()->GetOwnershipChangeQueue();

    Update();
}

void BorderModel::Update()
{
    auto &changeQueue = world::WorldScene::Get()->GetOwnershipChangeQueue();

    auto &edges = tiles.GetEdges();
    //std::cout<<edges.GetWidth()<<"-----"<<edges.GetHeight()<<"\n";
    /*for(auto y = 0; y < edges.GetHeight(); ++y)
    {
        for(auto x = 0; x < edges.GetWidth(); ++x)
        {
            auto edge = edges.Get(x, y);
            if(edge->First != nullptr)
                std::cout<<"x";
            else
                std::cout<<"-";
        }
        std::cout<<"\n";
    }*/

    for(auto &tile : changeQueue)
    {
        auto borderTile = tiles.GetTile(tile->HexCoordinates);

        for(auto &neighbour : borderTile->Neighbours)
        {
            if(neighbour.Border == nullptr)
                continue;
            
            auto border = neighbour.Border;
            auto determineBorder = [border] (WorldTile *firstTile, WorldTile *secondTile, BorderData &borderData)
            {
                if(firstTile->IsOwned() == false)
                {
                    borderData.IsInitialized = false;
                    return;
                }

                if(secondTile->IsOwned() == true && firstTile->GetOwner() == secondTile->GetOwner())
                {
                    borderData.IsInitialized = false;
                    return;
                }

                bool isHardBorder = secondTile->IsOwned() == false || secondTile->GetOwner()->GetPolity() != firstTile->GetOwner()->GetPolity();

                auto position = (firstTile->Position + secondTile->Position) / 2.0f;
                if(isHardBorder == true)
                {
                    position = (position + firstTile->Position * 0.5f) / 1.5f;
                }
                
                auto length = WorldMap::WORLD_TILE_SIZE * 1.0f;
                auto thickness = isHardBorder == true ? 12.0f : 5.0f;

                auto rotation = border->Rotation;
                
                auto color = firstTile->GetOwner()->GetRulerBanner();

                borderData = {position, rotation, thickness, length, color, true};
            };

            determineBorder(tile, neighbour.Neighbour->Tile, border->FirstData);
            determineBorder(neighbour.Neighbour->Tile, tile, border->SecondData);
        }
    }
}

void BorderModel::TransferData()
{
    positions.Reset();

    rotations.Reset();

    lengths.Reset();

    thicknesses.Reset();

    colors.Reset();

    for(auto border = tiles.GetEdges().GetStart(); border != tiles.GetEdges().GetEnd(); ++border)
    {
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

    (*buffers_.Get("Position"))->UploadData(positions.GetStart(), positions.GetMemorySize());

    (*buffers_.Get("Rotation"))->UploadData(rotations.GetStart(), rotations.GetMemorySize());

    (*buffers_.Get("Lengths"))->UploadData(lengths.GetStart(), lengths.GetMemorySize());

    (*buffers_.Get("Thickness"))->UploadData(thicknesses.GetStart(), thicknesses.GetMemorySize());

    (*buffers_.Get("Color"))->UploadData(colors.GetStart(), colors.GetMemorySize());
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

    glDrawArrays(GL_TRIANGLES, 0, 6 * positions.GetSize());

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