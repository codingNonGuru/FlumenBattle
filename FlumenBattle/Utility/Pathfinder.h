#pragma once

#include <concepts>

#include "FlumenCore/Singleton.h"
#include "FlumenCore/Container/HexGrid.h"
#include "FlumenCore/Container/Graph.h"

#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/WorldMap.h"
#include "FlumenBattle/Types.hpp"

#include <chrono>
using namespace std::chrono;

namespace utility
{
    template<typename T>
	concept CanBeTravelled = requires(T a, int b)
	{
        { a.GetNearbyTiles(b) };

        { a.IsBlocked() };

        { a.GetTravelPenalty() } -> std::same_as <TravelPenalty>;
	};

    template <class TileType> requires CanBeTravelled <TileType>
    class Pathfinder : public core::Singleton <Pathfinder <TileType>>
    {
        friend class core::Singleton <Pathfinder <TileType>>;

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

        struct PathData
        {
            const Array <TileData> &Tiles;

            Integer Complexity;

            Integer Length;
        };

        struct SettlementData
        {
            world::settlement::Settlement *Settlement;

            Integer Distance;

            Integer Complexity;

            bool IsFringe;

            SettlementData() {}

            SettlementData(world::settlement::Settlement *settlement) : Settlement(settlement) {}

            SettlementData(world::settlement::Settlement *settlement, Integer distance) : Settlement(settlement), Distance(distance), IsFringe(false) {}
        };

        struct SettlementPathData
        {
            const Array <SettlementData> &Settlements;

            Integer Complexity;

            Integer Length;
        };

        typedef container::Graph <TileData, 6> TileGraph;
        typedef container::Graph <SettlementData, 8> SettlementGraph;

        Array <TileData> visitedTiles;

        TileGraph tilePaths;

        Array <SettlementData> visitedSettlements;

        SettlementGraph settlementPaths;

        struct TileNodeData : public core::hex::Tile
        {
            TileGraph::Node *Node;
        };

        container::HexGrid <TileNodeData> nodeMap;

        Pathfinder()
        {
            visitedTiles = Array <TileData>(4096);

            tilePaths = TileGraph(4096);

            visitedSettlements = Array <SettlementData>(256);
            settlementPaths = SettlementGraph(4096);

            auto map = world::WorldScene::Get()->GetWorldMap();
            nodeMap.Initialize(map->GetSize(), map->GetSize());

            auto &worldTiles = map->GetTiles();
            for(auto tile = worldTiles.GetStart(); tile != worldTiles.GetEnd(); ++tile)
            {
                nodeMap.GetTile(tile->HexCoordinates)->Coordinates = tile->HexCoordinates;
            }
        }

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

        const Array <TileData> &MapArea(TileType *centerTile, Integer range = 7)
        {
            auto start = high_resolution_clock::now();

            auto &tiles = centerTile->GetNearbyTiles(range);
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
                TileData bestTile = {nullptr, 0};
                
                for(auto &tile : visitedTiles)
                {
                    searches++;
                    if(tile.Tile->PathData.IsVisited == false)
                        continue;

                    auto &nearbyTiles = tile.Tile->GetNearbyTiles();
                    for(auto nearbyTile = nearbyTiles.GetStart(); nearbyTile != nearbyTiles.GetEnd(); ++nearbyTile)
                    {
                        if((*nearbyTile)->PathData.IsVisited == false && (*nearbyTile)->PathData.IsToBeVisited == true)
                        {
                            auto penalty = GetPenalty(*nearbyTile);
                            if(tile.Distance + penalty < bestComplexity)
                            {
                                bestComplexity = tile.Distance + penalty;
                                bestTile.Tile = *nearbyTile;
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

            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);
            std::cout <<"map area duration " << duration.count() << "\n";

            return visitedTiles;
        }

        const Array <TileData> &MapAreaImproved(TileType *centerTile, Integer range = 7)
        {
            auto start = high_resolution_clock::now();

            auto &tiles = centerTile->GetNearbyTiles(range);
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
            TileData championTile = {centerTile, 0};
            while(true)
            {
                auto bestComplexity = INT_MAX;
                TileData bestTile = {nullptr, 0};
                
                for(auto &tile : visitedTiles)
                {
                    searches++;
                    if(tile.Tile->PathData.IsVisited == false)
                        continue;

                    auto &nearbyTiles = tile.Tile->GetNearbyTiles();
                    for(auto nearbyTile = nearbyTiles.GetStart(); nearbyTile != nearbyTiles.GetEnd(); ++nearbyTile)
                    {
                        if((*nearbyTile)->PathData.IsVisited == false && (*nearbyTile)->PathData.IsToBeVisited == true)
                        {
                            auto penalty = GetPenalty(*nearbyTile);
                            if(tile.Distance + penalty < bestComplexity)
                            {
                                bestComplexity = tile.Distance + penalty;
                                bestTile.Tile = *nearbyTile;

                                if(penalty == 1 && championTile.Distance + 1 == bestComplexity)
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

        SettlementPathData FindPathToSettlement(world::settlement::Settlement *start, world::settlement::Settlement *end)
        {
            if(start == end)
            {
                visitedSettlements.Reset();
                *visitedSettlements.Add() = start;

                return {visitedSettlements, 0, visitedSettlements.GetSize()};
            }

            settlementPaths.Clear();

            auto &settlements = world::WorldScene::Get()->GetSettlements();
            for(auto &settlement : settlements)
            {
                settlement.GetPathData().IsVisited = false;
                settlement.GetPathData().IsToBeVisited = true;
            }

            start->GetPathData().IsVisited = true;
            start->GetPathData().IsToBeVisited = true;

            visitedSettlements.Reset();
            *visitedSettlements.Add() = start;

            typename SettlementGraph::Node *championPath = settlementPaths.StartGraph({start, 0});

            int searches = 0;
            while(true)
            {
                auto bestComplexity = INT_MAX;
                typename SettlementGraph::Node *bestNode = nullptr;
                world::settlement::Settlement *bestSettlement = nullptr;
                
                for(auto &settlement : visitedSettlements)
                {
                    searches++;
                    if(settlement.Settlement->GetPathData().IsVisited == true)
                    {
                        auto &links = settlement.Settlement->GetLinks();
                        for(auto &link : links)
                        {
                            auto other = link.Path->GetOther(settlement.Settlement);
                            if(other->GetPathData().IsVisited == false && other->GetPathData().IsToBeVisited == true)
                            {
                                auto &nodes = settlementPaths.GetNodes();
                                for(auto &node : nodes)
                                {
                                    auto path = node.Content.Settlement->GetPathTo(other);
                                    if(path == nullptr)
                                        continue;
                                
                                    auto penalty = path->Complexity;
                                    if(node.Content.Distance + penalty < bestComplexity)
                                    {
                                        bestComplexity = node.Content.Distance + penalty;
                                        bestNode = &node;
                                        bestSettlement = other;
                                    }
                                }
                            }
                        }
                    }
                }

                auto newNode = bestNode->AddNode({bestSettlement, bestComplexity});
                championPath = newNode;
                bestSettlement->GetPathData().IsVisited = true;
                *visitedSettlements.Add() = bestSettlement;

                if(championPath->Content.Settlement == end)
                {
                    break;
                }
            }

            auto complexity = championPath->Content.Distance;
            visitedSettlements.Reset();
            while(true)
            {
                *visitedSettlements.Add() = championPath->Content.Settlement;

                championPath = championPath->GetPrevious();
                if(championPath == nullptr)
                {
                    return {visitedSettlements, complexity, visitedSettlements.GetSize()};
                }
            }
        }

        PathData FindPathDjikstraNew(TileType *startTile, TileType *endTile, Integer range = 7)
        {
            auto getPenalty = [] (TileType *tile)
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

            auto start = high_resolution_clock::now();

            tilePaths.Clear();

            auto coordinates = (startTile->SquareCoordinates + endTile->SquareCoordinates) / 2;
            auto middleTile = world::WorldScene::Get()->GetTiles().Get(coordinates.x, coordinates.y);

            auto &tiles = middleTile->GetNearbyTiles(range);
            for(auto &tile : tiles)
            {
                tile->PathData.IsVisited = false;
                tile->PathData.IsToBeVisited = true;
                tile->PathData.Node = nullptr;
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

                auto aloha = startTile->GetNearbyTiles();
                
                for(auto &tile : visitedTiles)
                {
                    if(tile.Tile->PathData.IsVisited == true)
                    {
                        auto &nearbyTiles = tile.Tile->GetNearbyTiles();
                        aloha = nearbyTiles;
                        for(auto nearbyTile = nearbyTiles.GetStart(); nearbyTile != nearbyTiles.GetEnd(); ++nearbyTile)
                        {
                            if((*nearbyTile)->PathData.IsVisited == false && (*nearbyTile)->PathData.IsToBeVisited == true)
                            {                                
                                auto nearbyTileNode = nodeMap.GetTile((*nearbyTile)->HexCoordinates);
                                auto &nearbyNodeMappings = nodeMap.GetNearbyTiles(nearbyTileNode, 1);
                                for(auto &nearbyNodeMapping : nearbyNodeMappings)
                                {
                                    if(nearbyNodeMapping == nearbyTileNode)
                                        continue;

                                    if(nearbyNodeMapping->Node == nullptr)
                                        continue;

                                    if(nearbyNodeMapping->Node->Content.Tile->PathData.IsToBeVisited == false)
                                        continue;

                                    //if(nearbyNodeMapping->Node->Content.Tile->PathData.IsVisited == false)
                                        //continue;

                                    searches++;
                                    auto penalty = getPenalty(*nearbyTile) + getPenalty(nearbyNodeMapping->Node->Content.Tile);

                                    if(nearbyNodeMapping->Node->Content.Distance + penalty < bestComplexity)
                                    {
                                        bestComplexity = nearbyNodeMapping->Node->Content.Distance + penalty;
                                        bestNode = nearbyNodeMapping->Node;
                                        bestTile = *nearbyTile;

                                        if(penalty == 2 && bestComplexity == championPath->Content.Distance + 2)
                                        {
                                            goto hasFoundShortcut;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                hasFoundShortcut:

                auto newNode = bestNode->AddNode({bestTile, bestComplexity});
                championPath = newNode;
                bestTile->PathData.IsVisited = true;
                *visitedTiles.Add() = bestTile;
                nodeMap.GetTile(bestTile->HexCoordinates)->Node = newNode;
                //bestTile->PathData.Node = newNode;

                if(championPath->Content.Tile == endTile)
                {
                    break;
                }
            }
            //std::cout<<"searches "<<searches<<"\n";
            //std::cout<<"length "<<startTile->GetDistanceTo(*endTile)<<"\n";
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);
            std::cout <<"duration " << duration.count() << "\n";

            auto complexity = championPath->Content.Distance;
            visitedTiles.Reset();
            while(true)
            {
                *visitedTiles.Add() = championPath->Content.Tile;

                championPath = championPath->GetPrevious();
                if(championPath == nullptr)
                {
                    return {visitedTiles, complexity / 2, visitedTiles.GetSize()};
                }
            }
        }

        PathData FindPathDjikstra(TileType *startTile, TileType *endTile, Integer range = 7)
        {
            auto getPenalty = [] (TileType *tile)
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

            auto start = high_resolution_clock::now();

            tilePaths.Clear();

            auto coordinates = (startTile->SquareCoordinates + endTile->SquareCoordinates) / 2;
            auto middleTile = world::WorldScene::Get()->GetTiles().Get(coordinates.x, coordinates.y);

            auto &tiles = middleTile->GetNearbyTiles(range);
            for(auto &tile : tiles)
            {
                tile->PathData.IsVisited = false;
                tile->PathData.IsToBeVisited = true;
                tile->PathData.Node = nullptr;
            }

            startTile->PathData.IsVisited = true;
            startTile->PathData.IsToBeVisited = true;

            visitedTiles.Reset();
            *visitedTiles.Add() = startTile;

            auto middleNode = nodeMap.GetTile(middleTile->HexCoordinates);
            auto &nearbyNodes = nodeMap.GetNearbyTiles(middleNode, range + 1);
            for(auto &node : nearbyNodes)
            //for(auto &node : nearbyNodes)
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

                auto aloha = startTile->GetNearbyTiles();
                
                for(auto &tile : visitedTiles)
                {
                    if(tile.Tile->PathData.IsVisited == true)
                    {
                        auto &nearbyTiles = tile.Tile->GetNearbyTiles();
                        aloha = nearbyTiles;
                        for(auto nearbyTile = nearbyTiles.GetStart(); nearbyTile != nearbyTiles.GetEnd(); ++nearbyTile)
                        {
                            if((*nearbyTile)->PathData.IsVisited == false && (*nearbyTile)->PathData.IsToBeVisited == true)
                            {                               
                                auto nearbyTileNode = nodeMap.GetTile((*nearbyTile)->HexCoordinates);
                                auto &nearbyNodeMappings = nodeMap.GetNearbyTiles(nearbyTileNode, 1);
                                for(auto &nearbyNodeMapping : nearbyNodeMappings)
                                {
                                    if(nearbyNodeMapping == nearbyTileNode)
                                        continue;

                                    if(nearbyNodeMapping->Node == nullptr)
                                        continue;

                                    if(nearbyNodeMapping->Node->Content.Tile->PathData.IsToBeVisited == false)
                                        continue;

                                    //if(nearbyNodeMapping->Node->Content.Tile->PathData.IsVisited == false)
                                        //continue;

                                    searches++;
                                    auto penalty = getPenalty(*nearbyTile) + getPenalty(nearbyNodeMapping->Node->Content.Tile);
                                    if(nearbyNodeMapping->Node->Content.Distance + penalty < bestComplexity)
                                    {
                                        bestComplexity = nearbyNodeMapping->Node->Content.Distance + penalty;
                                        bestNode = nearbyNodeMapping->Node;
                                        bestTile = *nearbyTile;
                                    }
                                }
                            }
                        }
                    }
                }

                auto newNode = bestNode->AddNode({bestTile, bestComplexity});
                championPath = newNode;
                bestTile->PathData.IsVisited = true;
                *visitedTiles.Add() = bestTile;
                nodeMap.GetTile(bestTile->HexCoordinates)->Node = newNode;
                //bestTile->PathData.Node = newNode;

                if(championPath->Content.Tile == endTile)
                {
                    break;
                }
            }

            //std::cout<<"searches "<<searches<<"\n";
            //std::cout<<"length "<<startTile->GetDistanceTo(*endTile)<<"\n";
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);
            std::cout <<"duration " << duration.count() << "\n";

            auto complexity = championPath->Content.Distance;
            visitedTiles.Reset();
            while(true)
            {
                *visitedTiles.Add() = championPath->Content.Tile;

                championPath = championPath->GetPrevious();
                if(championPath == nullptr)
                {
                    return {visitedTiles, complexity / 2, visitedTiles.GetSize()};
                }
            }
        }
    
        PathData FindPathDjikstraOld(TileType *startTile, TileType *endTile, Integer range = 7)
        {
            auto getPenalty = [] (TileType *tile)
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
            
            auto start = high_resolution_clock::now();

            tilePaths.Clear();

            auto coordinates = (startTile->SquareCoordinates + endTile->SquareCoordinates) / 2;
            auto middleTile = world::WorldScene::Get()->GetTiles().Get(coordinates.x, coordinates.y);

            auto &tiles = middleTile->GetNearbyTiles(range);
            for(auto &tile : tiles)
            {
                tile->PathData.IsVisited = false;
                tile->PathData.IsToBeVisited = true;
            }

            startTile->PathData.IsVisited = true;
            startTile->PathData.IsToBeVisited = true;

            visitedTiles.Reset();
            *visitedTiles.Add() = startTile;

            typename TileGraph::Node *championPath = tilePaths.StartGraph({startTile, 0});

            int searches = 0;
            while(true)
            {
                auto bestComplexity = INT_MAX;
                typename TileGraph::Node *bestNode = nullptr;
                TileType *bestTile = nullptr;
                
                for(auto &tile : visitedTiles)
                {
                    searches++;
                    if(tile.Tile->PathData.IsVisited == true)
                    {
                        auto &nearbyTiles = tile.Tile->GetNearbyTiles();
                        for(auto nearbyTile = nearbyTiles.GetStart(); nearbyTile != nearbyTiles.GetEnd(); ++nearbyTile)
                        {
                            if((*nearbyTile)->PathData.IsVisited == false && (*nearbyTile)->PathData.IsToBeVisited == true)
                            {
                                auto &nodes = tilePaths.GetNodes();
                                for(auto &node : nodes)
                                {
                                    if(node.Content.Tile->GetDistanceTo(**nearbyTile) == 1)
                                    {
                                        auto penalty = getPenalty(*nearbyTile) + getPenalty(node.Content.Tile);
                                        if(node.Content.Distance + penalty < bestComplexity)
                                        {
                                            bestComplexity = node.Content.Distance + penalty;
                                            bestNode = &node;
                                            bestTile = *nearbyTile;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                auto newNode = bestNode->AddNode({bestTile, bestComplexity});
                championPath = newNode;
                bestTile->PathData.IsVisited = true;
                *visitedTiles.Add() = bestTile;

                if(championPath->Content.Tile == endTile)
                {
                    break;
                }
            }

            //std::cout<<"searches "<<searches<<"\n";
            //std::cout<<"length "<<startTile->GetDistanceTo(*endTile)<<"\n";
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);
            std::cout <<"duration " << duration.count() << "\n\n";

            auto complexity = championPath->Content.Distance;
            visitedTiles.Reset();
            while(true)
            {
                *visitedTiles.Add() = championPath->Content.Tile;

                championPath = championPath->GetPrevious();
                if(championPath == nullptr)
                {
                    return {visitedTiles, complexity / 2, visitedTiles.GetSize()};
                }
            }
        }
    };
}