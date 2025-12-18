#include "FlumenEngine/Utility/Perlin.hpp"
#include "FlumenEngine/Render/Texture.hpp"

#include "RiverGenerator.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Tile/WorldMap.h"
#include "FlumenBattle/World/Tile/WorldTile.h"
#include "FlumenBattle/World/Tile/WorldBiome.h"
#include "FlumenBattle/World/Tile/WorldRelief.h"
#include "FlumenBattle/World/Tile/River.h"
#include "FlumenBattle/World/Render/RiverModel.h"

using namespace world;

#define MOUTH_DISTANCE 6

#define MOUTH_MINIMUM_SLOPE 10

#define SLOPE_VIABILITY_FACTOR 2

#define CARVE_ITERATION_COUNT 4096

struct DummyTile : core::hex::PhysicalTile {bool HasRiver;};

struct DummyEdge : core::hex::HexEdge <DummyTile> {bool HasRiver;};

container::EdgyHexGrid <DummyTile, DummyEdge> dummyEdgeGrid;

void RiverGenerator::DefineRivers()
{
    auto &scene = *WorldScene::Get();

    auto map = scene.GetWorldMap();

    dummyEdgeGrid.Initialize(map->GetTiles().GetWidth(), map->GetTiles().GetHeight());

    dummyEdgeGrid.AddEdges();

    /*auto areEdgesJoint = [&] (tile::WorldEdge &edge, tile::WorldEdge &otherEdge)
    {
        auto distances = {
            edge.First->GetDistanceTo(*otherEdge.First), 
            edge.First->GetDistanceTo(*otherEdge.Second),
            edge.Second->GetDistanceTo(*otherEdge.First), 
            edge.Second->GetDistanceTo(*otherEdge.Second), 
        };

        return std::accumulate(std::begin(distances), std::end(distances), 0, std::plus<int>()) == 2;
    };*/

    for(auto &tile : map->GetTiles())
    {
        tile.IsCoastline = false;

        tile.IsBay = false;
    }

    for(auto &tile : map->GetTiles())
    {
        auto seaCount = 0;

        auto landCount = 0;

        auto nearbyTiles = tile.GetNearbyTiles();
        for(auto &otherTile : nearbyTiles)
        {
            if(otherTile == nullptr)
                continue;

            auto edge = map->GetEdge(&tile, otherTile);
            edge->First = &tile;
            edge->Second = otherTile;

            if(tile.Type != otherTile->Type)
            {
                tile.IsCoastline = true;

                otherTile->IsCoastline = true;
            }

            if(otherTile->Type == WorldTiles::SEA)
                seaCount++;

            if(otherTile->Type == WorldTiles::LAND)
                landCount++;
        }

        if(tile.Type == WorldTiles::LAND && seaCount > 3)
            tile.IsPeninsula = true;
        else
            tile.IsPeninsula = false;

        if(tile.Type == WorldTiles::SEA && landCount > 3)
            tile.IsBay = true;
        else
            tile.IsBay = false;
    }

    for(auto &edge : map->GetEdges())
    {
        if(edge.First == nullptr || edge.Second == nullptr)
            continue;

        bool isFirst = true;

        auto nearbyTiles = edge.First->GetNearbyTiles();
        for(auto &neighbour : nearbyTiles)
        {
            if(neighbour == nullptr)
                continue;

            if(edge.Second->GetDistanceTo(*neighbour) != 1)
                continue;

            if(isFirst == true)
            {
                edge.Start = neighbour;
                isFirst = false;
            }
            else
            {
                edge.End = neighbour;
                break;
            }
        }
    }

    ComputeSlopes();

    EstablishRiverMouths();

    CarveUpstream();

    ComputeDischarge();

    EstablishCorners();
}

void RiverGenerator::ComputeSlopes()
{
    auto &scene = *WorldScene::Get();

    auto map = scene.GetWorldMap();

    auto getHeightDelta = [&] (tile::WorldEdge &edge)
    {
        struct FlowData
        {
            tile::WorldTile *LowerTile; 
            int Delta;
        };

        auto delta = (edge.End->Elevation - edge.First->Elevation)
            + (edge.End->Elevation - edge.Second->Elevation) 
            + (edge.First->Elevation - edge.Start->Elevation) 
            + (edge.Second->Elevation - edge.Start->Elevation);

        if(delta >= SLOPE_VIABILITY_FACTOR)
            return FlowData{edge.Start, delta};
        else if(delta <= -SLOPE_VIABILITY_FACTOR)
            return FlowData{edge.End, delta};
        else
            return FlowData{nullptr, delta};
    };

    for(auto &edge : map->GetEdges())
    {
        if(edge.First == nullptr || edge.Second == nullptr || edge.Start == nullptr || edge.End == nullptr)
            continue;

        auto flowData = getHeightDelta(edge);

        edge.Downstream = flowData.LowerTile;
        edge.HeightDelta = flowData.Delta;
    }
}

void RiverGenerator::EstablishRiverMouths()
{
    auto &scene = *WorldScene::Get();

    auto map = scene.GetWorldMap();

    auto clearDummies = [&] 
    {
        for(auto &edge : dummyEdgeGrid.GetEdges())
        {
            edge.HasRiver = false;
        }

        for(int x = 0; x < dummyEdgeGrid.GetTiles().GetWidth(); ++x)
            for(int y = 0; y < dummyEdgeGrid.GetTiles().GetHeight(); ++y)
            {
                auto tile = dummyEdgeGrid.GetTiles().Get(x, y);
                tile->HasRiver = false;
                tile->Coordinates = map->GetTile({x, y})->Coordinates;
            }
    };

    clearDummies();

    for(auto &edge : map->GetEdges())
    {
        if(edge.First == nullptr || edge.Second == nullptr || edge.Start == nullptr || edge.End == nullptr)
            continue;

        if(edge.First->Type == WorldTiles::SEA || edge.Second->Type == WorldTiles::SEA)
            continue;

        if(edge.First->IsPeninsula == true || edge.Second->IsPeninsula == true)
            continue;

        if(dummyEdgeGrid.GetTile(edge.First->SquareCoordinates)->HasRiver == true || dummyEdgeGrid.GetTile(edge.Second->SquareCoordinates)->HasRiver == true)
            continue;

        auto seaCount = GetNeighbouringSeaTiles(edge);

        if(seaCount != 1)
            continue;

        if(edge.Downstream == nullptr)
            continue;

        if(abs(edge.HeightDelta) < MOUTH_MINIMUM_SLOPE)
            continue;

        auto river = map->AddRiver(&edge);

        edge.River = river;

        auto dummyTile = dummyEdgeGrid.GetTile(edge.First->SquareCoordinates);
        auto nearbyDummies = dummyEdgeGrid.GetNearbyTiles(dummyTile, MOUTH_DISTANCE);

        for(auto &tile : nearbyDummies)
        {
            tile->HasRiver = true;
        }
    }
}

void RiverGenerator::CarveUpstream()
{
    auto &scene = *WorldScene::Get();

    auto map = scene.GetWorldMap();

    auto getNeighbouringEdges = [&] (tile::WorldEdge &edge)
    {
        auto edges = std::array{
            map->GetEdge(edge.First, edge.Start), map->GetEdge(edge.First, edge.End), map->GetEdge(edge.Second, edge.Start), map->GetEdge(edge.Second, edge.End)
        };

        return edges;
    };

    auto isFlowingInSameDirection = [&] (tile::WorldEdge &edge, tile::WorldEdge &otherEdge)
    {
        return edge.Downstream->GetDistanceTo(*otherEdge.Downstream) == 1 && edge.GetUpstream()->GetDistanceTo(*otherEdge.GetUpstream()) == 1;
    };

    auto isAtJunction = [&] (tile::WorldEdge &edge, tile::River &river)
    {
        auto isUpstream = map->GetEdge(edge.First, edge.GetUpstream())->River == &river && map->GetEdge(edge.Second, edge.GetUpstream())->River == &river;
        auto isDownstream = map->GetEdge(edge.First, edge.Downstream)->River == &river && map->GetEdge(edge.Second, edge.Downstream)->River == &river;

        return isUpstream || isDownstream;
    };

    for(int i = 0; i < CARVE_ITERATION_COUNT; ++i)
    {
        auto river = map->GetRivers().GetRandom();

        auto source = *(river->GetSegments().GetLast() - 1);

        container::SmartBlock <tile::WorldEdge *, 16> candidates;

        auto neighbours = getNeighbouringEdges(*source);

        for(auto &neighbour : neighbours)
        {
            if(neighbour->River != nullptr)
                continue;

            if(neighbour->Downstream == nullptr)
                continue;

            if(neighbour->First->Type == WorldTiles::SEA || neighbour->Second->Type == WorldTiles::SEA)
                continue;

            auto farNeighbours = getNeighbouringEdges(*neighbour);
            bool hasFound = false;
            for(auto &farNeighbour : farNeighbours)
            {
                if(farNeighbour->River != river && farNeighbour->River != nullptr)
                {
                    hasFound = true;
                    break;
                }
            }

            if(hasFound == true)
                continue;

            if(isFlowingInSameDirection(*source, *neighbour) == false)
                continue;

            if(isAtJunction(*neighbour, *river) == true)
                continue;

            *candidates.Add() = neighbour;
        }

        /*auto biggestSlope = 0;
        tile::WorldEdge *bestCandidate = nullptr;
        for(auto &candidate : candidates)
        {
            if(abs(candidate->HeightDelta) > biggestSlope)
            {
                biggestSlope = abs(candidate->HeightDelta);
                bestCandidate = candidate;
            }
        }*/

        if(candidates.GetSize() == 0)
            continue;

        auto bestCandidate = *candidates.GetRandom();

        if(bestCandidate == nullptr)
            continue;

        bestCandidate->River = river;
        river->AddSegment(bestCandidate);
    }

    /*for(int i = 0; i < 30; ++i)
    {
        for(auto &edge : map->GetEdges())
        {
            if(edge.River != nullptr)
                continue;

            if(edge.First == nullptr || edge.Second == nullptr || edge.Start == nullptr || edge.End == nullptr)
                continue;

            if(edge.First->IsPeninsula == true || edge.Second->IsPeninsula == true)
                continue;

            auto seaCount = GetNeighbouringSeaTiles(edge);

            if(seaCount != 0)
                continue;

            if(edge.Downstream == nullptr)
                continue;

            auto neighbours = getNeighbouringEdges(edge);

            tile::River *adjacentRiver = nullptr;

            auto adjacentRiverCount = 0;
            for(auto &neighbour : neighbours)
                if(neighbour->River != nullptr) 
                {
                    adjacentRiverCount++;

                    adjacentRiver = neighbour->River;
                }
                
            if(adjacentRiverCount == 0 || adjacentRiverCount == 4 || adjacentRiverCount == 3 || adjacentRiverCount == 2)
                continue;

            bool isFlowingInOtherDirection = false;
            for(auto &neighbour : neighbours)
            {
                if(neighbour->River != nullptr && isFlowingInSameDirection(edge, *neighbour) == false)
                    isFlowingInOtherDirection = true;
            }

            if(isFlowingInOtherDirection == true)
                continue;

            edge.River = adjacentRiver;

            adjacentRiver->AddSegment(&edge);
        }
    }*/
}

void RiverGenerator::ComputeDischarge()
{
    auto &scene = *WorldScene::Get();

    auto map = scene.GetWorldMap();

    for(auto &river : map->GetRivers())
    {
        auto &segments = river.GetSegments();
        int discharge = segments.GetSize();
        for(auto &segment : segments)
        {
            segment->Discharge = discharge;
            discharge--;
        }
    }
}

void RiverGenerator::EstablishCorners()
{
    auto map = WorldScene::Get()->GetWorldMap();

    for(auto &river : map->GetRivers())
    {
        for(auto i = 0; i <= river.GetSegments().GetSize(); ++i)
        {
            auto segment = i == river.GetSegments().GetSize() ? nullptr : *river.GetSegments().Get(i);

            auto previousSegment = i == 0 ? nullptr : *river.GetSegments().Get(i - 1);

            auto twist = river.AddTwist();

            twist->First = previousSegment;

            twist->Second = segment;

            if(twist->First == nullptr || twist->Second == nullptr)
                continue;
            
            tile::WorldTile *tiles[3] = 
            {
                twist->First->First, 
                twist->First->Second, 
                twist->Second->First != twist->First->First && twist->Second->First != twist->First->Second ? twist->Second->First : twist->Second->Second
            };

            twist->Position = (tiles[0]->Position + tiles[1]->Position + tiles[2]->Position) / 3.0f;
        }

        auto startTwist = river.GetTwists().Get(0);
        auto edgePosition = (startTwist->Second->First->Position + startTwist->Second->Second->Position) / 2.0f;
        auto nextTwistPosition = river.GetTwists().Get(1)->Position;

        startTwist->Position = (edgePosition - nextTwistPosition) * 2.0f + edgePosition;

        auto endTwist = river.GetTwists().GetLast() - 1;
        edgePosition = (endTwist->First->First->Position + endTwist->First->Second->Position) / 2.0f;
        auto previousTwistPosition = (river.GetTwists().GetLast() - 2)->Position;

        endTwist->Position = (edgePosition - previousTwistPosition) + edgePosition;
    }
}

void RiverGenerator::GenerateDistortionMaps()
{
    if(render::RiverModel::Get()->angleTexture != nullptr)
        return;

    static container::Grid <float> noise(1024, 1024);

    Perlin::Generate(noise.GetSize(), 0.2f, ContrastThreshold(0.5f), ContrastStrength(2.0f));
    Perlin::Download(&noise);

    render::RiverModel::Get()->angleTexture = new ::render::Texture(noise.GetSize(), TextureFormats::ONE_FLOAT, &noise);

    Perlin::Generate(noise.GetSize(), 0.2f, ContrastThreshold(0.5f), ContrastStrength(2.0f));
    Perlin::Download(&noise);

    render::RiverModel::Get()->radiusTexture = new ::render::Texture(noise.GetSize(), TextureFormats::ONE_FLOAT, &noise);
}

int RiverGenerator::GetNeighbouringSeaTiles(tile::WorldEdge &edge)
{
    auto seaCount = 0;

    auto types = {edge.First->Type, edge.Second->Type, edge.Start->Type, edge.End->Type};
    for(auto &type : types)
    {
        if(type == WorldTiles::SEA)
            seaCount++;
    }

    return seaCount;
};
