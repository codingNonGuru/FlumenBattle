#pragma once

#include <concepts>
#include <chrono>

#include "FlumenCore/Singleton.h"
#include "FlumenCore/Container/HexGrid.h"
#include "FlumenCore/Container/Graph.h"

#include "FlumenBattle/World/WorldMap.h"
#include "FlumenBattle/Utility/Types.h"

using namespace std::chrono;

namespace utility
{
    template <typename TileType>
    struct PathData;

    template <class TileType> requires CanBeTravelled <TileType>
    struct TileData
    {
        TileType *Tile;

        Integer Distance;

        Integer Complexity;

        bool IsFringe;

        TileData() {}

        TileData(TileType *tile) : Tile(tile) {}

        TileData(TileType *tile, Integer distance) : Tile(tile), Distance(distance), IsFringe(false) {}
    };

    struct GenericPathData {};

    template <class TileType> requires CanBeTravelled <TileType>
    struct PathfindResult : public GenericPathData
    {
        const Array <TileData <TileType>> &Tiles;

        Integer Complexity;

        Integer Length;
    };

    template <class TileType> requires CanBeTravelled <TileType>
    class Pathfinder : public core::Singleton <Pathfinder <TileType>>
    {
        friend class core::Singleton <Pathfinder <TileType>>;

    private:
        typedef container::Graph <TileData <TileType>, 6> TileGraph;

        Array <TileData <TileType>> visitedTiles;

        TileGraph tilePaths;

        struct TileNodeData : public core::hex::Tile
        {
            TileGraph::Node *Node;
        };

        container::HexGrid <TileNodeData> nodeMap;

        Pathfinder();

    public:
        int GetPenalty(TileType *tile)
        {
            auto penalties = tile->GetTravelPenalty();
            if(penalties.Penalties.Find(TravelPenaltyTypes::SEA) != nullptr)
            {
                return 10;
            }
            else if(penalties.Penalties.Find(TravelPenaltyTypes::MOUNTAINS) != nullptr)
            {
                return 5;
            }
            else if(penalties.Penalties.Find(TravelPenaltyTypes::WOODS) != nullptr)
            {
                return 3;
            }
            else
            {
                return 1;
            }
        };

        const Array <TileData <TileType>> &MapAreaOld(TileType *centerTile, Integer range = 7)
        {
            auto start = steady_clock::now();

            auto tiles = centerTile->GetNearbyTiles(range);
            for(auto &tile : tiles)
            {
                tile->PathData.IsVisited = false;
                tile->PathData.IsToBeVisited = true;
            }

            centerTile->PathData.IsVisited = true;
            centerTile->PathData.IsToBeVisited = true;

            visitedTiles.Reset();
            *visitedTiles.Add() = {centerTile, 0};

            int searches = 0;
            while(true)
            {
                auto bestComplexity = INT_MAX;
                TileData <TileType> bestTile = {nullptr, 0};
                
                for(auto &tile : visitedTiles)
                {
                    searches++;
                    if(tile.Tile->PathData.IsVisited == false)
                        continue;

                    auto nearbyTiles = tile.Tile->GetNearbyTiles();
                    for(auto &nearbyTile : nearbyTiles)
                    {
                        if(nearbyTile->PathData.IsVisited == false && nearbyTile->PathData.IsToBeVisited == true)
                        {
                            auto penalty = GetPenalty(nearbyTile);
                            if(tile.Distance + penalty < bestComplexity)
                            {
                                bestComplexity = tile.Distance + penalty;
                                bestTile.Tile = nearbyTile;
                            }
                        }
                    }
                }

                bestTile.Tile->PathData.IsVisited = true;
                *visitedTiles.Add() = {bestTile.Tile, bestComplexity};

                if(tiles.GetSize() == visitedTiles.GetSize())
                {
                    break;
                }
            }

            auto stop = steady_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);
            std::cout << "searches " << searches << "\n";
            std::cout << "map area duration " << duration.count() << "\n";

            return visitedTiles;
        }

        const Array <TileData <TileType>> &MapAreaImproved(TileType *centerTile, Integer range = 7)
        {
            auto start = high_resolution_clock::now();

            auto tiles = centerTile->GetNearbyTiles(range);
            for(auto &tile : tiles)
            {
                tile->PathData.IsVisited = false;
                tile->PathData.IsToBeVisited = true;
            }

            centerTile->PathData.IsVisited = true;
            centerTile->PathData.IsToBeVisited = true;

            visitedTiles.Reset();
            *visitedTiles.Add() = {centerTile, 0};

            int searches = 0;
            TileData <TileType> championTile = {centerTile, 0};
            while(true)
            {
                auto bestComplexity = INT_MAX;
                TileData <TileType> bestTile = {nullptr, 0};
                
                for(auto &tile : visitedTiles)
                {
                    searches++;
                    if(tile.Tile->PathData.IsVisited == false)
                        continue;

                    auto nearbyTiles = tile.Tile->GetNearbyTiles();
                    for(auto &nearbyTile : nearbyTiles)
                    {
                        if(nearbyTile->PathData.IsVisited == false && nearbyTile->PathData.IsToBeVisited == true)
                        {
                            auto penalty = GetPenalty(nearbyTile);
                            if(tile.Distance + penalty < bestComplexity)
                            {
                                bestComplexity = tile.Distance + penalty;
                                bestTile.Tile = nearbyTile;

                                if(penalty == 1 && championTile.Distance == bestComplexity)
                                {
                                    goto hasFoundShortcut;
                                }
                            }
                        }
                    }
                }

                hasFoundShortcut:

                bestTile.Tile->PathData.IsVisited = true;
                *visitedTiles.Add() = {bestTile.Tile, bestComplexity};

                championTile = {bestTile.Tile, bestComplexity};

                if(tiles.GetSize() == visitedTiles.GetSize())
                {
                    break;
                }
            }

            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);
            std::cout <<"map area duration " << duration.count() << "\n";

            return visitedTiles;
        }

        const Array <TileData <TileType>> &MapArea(TileType *centerTile, Integer range = 7)
        {
            auto start = high_resolution_clock::now();

            auto tiles = centerTile->GetNearbyTiles(range);
            for(auto &tile : tiles)
            {
                tile->PathData.IsVisited = false;
                tile->PathData.IsToBeVisited = true;
                tile->PathData.IsTotallyVisited = false;
            }

            centerTile->PathData.IsVisited = true;
            centerTile->PathData.IsToBeVisited = true;

            visitedTiles.Reset();
            *visitedTiles.Add() = {centerTile, 0};

            int searches = 0;
            TileData <TileType> championTile = {centerTile, 0};
            while(true)
            {
                auto bestComplexity = INT_MAX;
                TileData <TileType> bestTile = {nullptr, 0};
                
                for(auto &tile : visitedTiles)
                {
                    searches++;
                    if(tile.Tile->PathData.IsVisited == false)
                        continue;

                    if(tile.Tile->PathData.IsTotallyVisited == true)
                        continue;

                    auto visitedNeighbourCount = 0;

                    auto nearbyTiles = tile.Tile->GetNearbyTiles();
                    for(auto &nearbyTile : nearbyTiles)
                    {
                        if(nearbyTile->PathData.IsVisited == false && nearbyTile->PathData.IsToBeVisited == true)
                        {
                            auto penalty = GetPenalty(nearbyTile);
                            if(tile.Distance + penalty < bestComplexity)
                            {
                                bestComplexity = tile.Distance + penalty;
                                bestTile.Tile = nearbyTile;

                                if(penalty == 1 && championTile.Distance == bestComplexity)
                                {
                                    goto hasFoundShortcut;
                                }
                            }
                        }
                        else if(nearbyTile->PathData.IsVisited == true && nearbyTile->PathData.IsToBeVisited == true)
                        {
                            visitedNeighbourCount++;
                        }
                    }

                    if(visitedNeighbourCount == 6)
                    {
                        tile.Tile->PathData.IsTotallyVisited = true;
                    }
                }

                hasFoundShortcut:

                bestTile.Tile->PathData.IsVisited = true;
                *visitedTiles.Add() = {bestTile.Tile, bestComplexity};

                championTile = {bestTile.Tile, bestComplexity};

                if(tiles.GetSize() == visitedTiles.GetSize())
                {
                    break;
                }
            }

            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);
            std::cout <<"map area duration " << duration.count() << "\n";

            return visitedTiles;
        }

        PathData <TileType> FindPathAsGeneric(TileType *startTile, TileType *endTile, Integer range = 7)
        {
            auto pathData = FindPathDjikstra(startTile, endTile, range);
            return {&pathData};
        }

        PathfindResult <TileType> FindPathDjikstra(TileType *startTile, TileType *endTile, Integer range = 7)
        {
            if(startTile == endTile)
            {
                visitedTiles.Reset();
                return {{}, visitedTiles, 0, 0};
            }

            auto start = high_resolution_clock::now();

            tilePaths.Clear();

            auto coordinates = (startTile->HexCoordinates + endTile->HexCoordinates) / 2;
            TileType *middleTile = startTile->GetNeighbor(endTile->HexCoordinates - coordinates);

            auto tiles = middleTile->GetNearbyTiles(range);
            for(auto &tile : tiles)
            {
                tile->PathData.IsVisited = false;
                tile->PathData.IsToBeVisited = true;
                tile->PathData.IsTotallyVisited = false;
            }

            auto ring = middleTile->GetTileRing(range + 1);
            for(auto &tile : ring)
            {
                tile->PathData.IsVisited = true;
                tile->PathData.IsToBeVisited = true;
            }

            startTile->PathData.IsVisited = true;
            startTile->PathData.IsToBeVisited = true;

            visitedTiles.Reset();
            *visitedTiles.Add() = startTile;

            auto middleNode = nodeMap.GetTile(middleTile->HexCoordinates);
            auto &nearbyNodes = nodeMap.GetNearbyTiles(middleNode, range + 1);
            for(auto &node : nearbyNodes)
            {
                node->Node = nullptr;
            }

            typename TileGraph::Node *championPath = tilePaths.StartGraph({startTile, 0});

            auto startNode = nodeMap.GetTile(startTile->HexCoordinates);
            startNode->Node = championPath;
            //startTile->PathData.Node = (void *)championPath;

            int searches = 0;
            while(true)
            {
                auto bestComplexity = INT_MAX;
                typename TileGraph::Node *bestNode = nullptr;
                TileType *bestTile = nullptr;

                for(auto &tile : visitedTiles)
                {
                    if(tile.Tile->PathData.IsVisited == false)
                        continue;

                    if(tile.Tile->PathData.IsTotallyVisited == true)
                        continue;

                    auto visitedNeighbourCount = 0;

                    auto nearbyTiles = tile.Tile->GetNearbyTiles();
                    for(auto nearbyTile : nearbyTiles)
                    {
                        if(nearbyTile->PathData.IsVisited == false && nearbyTile->PathData.IsToBeVisited == true)
                        {                               
                            auto nearbyTileNode = nodeMap.GetTile(nearbyTile->HexCoordinates);
                            auto nearbyNodeMappings = nodeMap.GetNearbyTiles(nearbyTileNode);
                            for(auto &nearbyNodeMapping : nearbyNodeMappings)
                            {
                                if(nearbyNodeMapping == nearbyTileNode)
                                    continue;

                                if(nearbyNodeMapping->Node == nullptr)
                                    continue;

                                if(nearbyNodeMapping->Node->Content.Tile->PathData.IsToBeVisited == false)
                                    continue;

                                searches++;
                                auto penalty = nearbyTile->GetPenalty() + nearbyNodeMapping->Node->Content.Tile->GetPenalty();
                                if(nearbyNodeMapping->Node->Content.Distance + penalty < bestComplexity)
                                {
                                    bestComplexity = nearbyNodeMapping->Node->Content.Distance + penalty;
                                    bestNode = nearbyNodeMapping->Node;
                                    bestTile = nearbyTile;

                                    if(penalty == 2 && bestComplexity == championPath->Content.Distance)
                                    {
                                        goto hasFoundShortcut;
                                    }
                                }
                            }
                        }
                        else if(nearbyTile->PathData.IsVisited == true && nearbyTile->PathData.IsToBeVisited == true)
                        {
                            visitedNeighbourCount++;
                        }
                    }

                    if(visitedNeighbourCount == 6)
                    {
                        tile.Tile->PathData.IsTotallyVisited = true;
                    }
                }

                hasFoundShortcut:

                auto newNode = bestNode->AddNode({bestTile, bestComplexity});
                championPath = newNode;
                bestTile->PathData.IsVisited = true;
                *visitedTiles.Add() = bestTile;
                nodeMap.GetTile(bestTile->HexCoordinates)->Node = newNode;

                if(championPath->Content.Tile == endTile)
                {
                    break;
                }
            }

            //std::cout<<"searches "<<searches<<"\n";
            //std::cout<<"length "<<startTile->GetDistanceTo(*endTile)<<"\n";
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);
            //std::cout <<"duration " << duration.count() << "\n";

            auto complexity = championPath->Content.Distance;
            visitedTiles.Reset();
            while(true)
            {
                *visitedTiles.Add() = championPath->Content.Tile;

                championPath = championPath->GetPrevious();
                if(championPath == nullptr)
                {
                    return {{}, visitedTiles, complexity / 2, visitedTiles.GetSize()};
                }
            }
        }
    };

    template <class TileType>
    struct PathData
    {
        container::SmartBlock <TileType *, 32> Tiles;

        Integer Complexity {0};

        Integer Length {0};

        PathData() {}

        PathData(GenericPathData *);

        /*PathData(const utility::PathData <TileType> &pathData)
        {
            for(auto &tile : pathData.Tiles)
            {
                *Tiles.Add() = tile.Tile;
            }

            Complexity = pathData.Complexity;

            Length = pathData.Length;
        }*/

        /*PathData(const utility::PathData <TileType> &pathData)
        {
            for(auto &tile : pathData.Tiles)
            {
                *Tiles.Add() = tile.Tile;
            }

            Complexity = pathData.Complexity;

            Length = pathData.Length;
        }*/
    };
}