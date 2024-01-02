#pragma once

#include <concepts>

#include "FlumenCore/Singleton.h"
#include "FlumenCore/Container/HexGrid.h"
#include "FlumenCore/Container/Graph.h"

#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/Types.hpp"

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

        struct TraversalData
        {
            bool IsPathValid;

            Integer Complexity;
        };

        struct PathData
        {
            const Array <TileData> &Tiles;

            Integer Complexity;

            Integer Length;
        };

        typedef container::Graph <TileData, 6> GraphType;
        typedef container::Graph <TileData, 6> ImprovedGraphType;

        Array <TileData> visitedTiles;

        Array <TileData> freshlyVisitedTiles;

        Array <Array <TileData>> fringeTiles;

        //Array <TileType *> 

        //GraphType paths;

        ImprovedGraphType firstPaths;

        ImprovedGraphType secondPaths;

        Array <TileData> firstOptimalPath;

        Array <TileData> secondOptimalPath;

    public:
        Pathfinder()
        {
            visitedTiles = Array <TileData>(1024);

            freshlyVisitedTiles = Array <TileData>(1024);

            fringeTiles = [] () {
                static auto arrays = Array <Array <TileData>>(128);

                for(int i = 0; i < arrays.GetCapacity(); ++i)
                {
                    *arrays.Add() = Array <TileData> (128);
                }

                return arrays;
            } ();

            //paths = GraphType(1024);

            firstPaths = ImprovedGraphType(65536);
            secondPaths = ImprovedGraphType(65536);

            firstOptimalPath = Array <TileData>(1024);
            secondOptimalPath = Array <TileData>(1024);
        }

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
                            //auto penalty = GetPenalty(*nearbyTile) + GetPenalty(tile.Tile);
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

            return visitedTiles;
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

            firstPaths.Clear();

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

            typename ImprovedGraphType::Node *championPath = firstPaths.StartGraph({startTile, 0});

            int searches = 0;
            while(true)
            {
                auto bestComplexity = INT_MAX;
                typename ImprovedGraphType::Node *bestNode = nullptr;
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
                                auto &nodes = firstPaths.GetNodes();
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
            //std::cout<<"complexity "<<championPath->Content.Distance<<"\n";

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

        void PassThroughTiles(TileType *centerTile, Integer range, Integer passIndex)
        {
            auto getPenalty = [] (TileType *tile)
            {
                if(tile->GetTravelPenalty() > 0)
                {
                    return 5;
                }

                return 1;
            };

            auto &targetTiles = centerTile->GetNearbyTiles(range);
            for(auto &tile : targetTiles)
            {
                tile->PathData.IsVisited = false;
                tile->PathData.IsFreshlyVisited = false;
            }

            centerTile->PathData.IsVisited = true;
            centerTile->PathData.IsFreshlyVisited = true;
            centerTile->PathData.Passes[passIndex] = getPenalty(centerTile);

            for(int index = 0; index <= range; ++index)
            {
                auto &fringeTiles = centerTile->GetTileRing(index);

                for(auto &tile : fringeTiles)
                {
                    tile->PathData.IsVisited = true;
                    tile->PathData.IsFreshlyVisited = false;
                }

                for(auto &tile : fringeTiles)
                {
                    auto &neighbours = tile->GetNearbyTiles(1, 1);

                    for(auto neighbour : neighbours)
                    {
                        if(neighbour->PathData.IsVisited == true || neighbour->PathData.IsFreshlyVisited == true)
                            continue;

                        auto &newNeighbours = neighbour->GetNearbyTiles(1, 2);
                        auto lowestScore = INT_MAX;
                        for(auto newNeighbour : newNeighbours)
                        {
                            if(newNeighbour->PathData.IsVisited == true)
                            {
                                if(newNeighbour->PathData.Passes[passIndex] < lowestScore)
                                {
                                    lowestScore = newNeighbour->PathData.Passes[passIndex];
                                }
                            }
                        }

                        auto weight = lowestScore + getPenalty(neighbour);
                        neighbour->PathData.IsFreshlyVisited = true;
                        neighbour->PathData.Passes[passIndex] = weight;

                        /*auto outerFringe = fringeTiles.Get(k);
                        *outerFringe->Add() = {neighbour, weight};*/
                    }
                }
            }

            /*auto &tilesss = centerTile->GetNearbyTiles(range);
            for(auto &tile : tilesss)
            {
                std::cout<<tile->PathData.Passes[passIndex]<<" "<<tile->SquareCoordinates.x<<" "<<tile->SquareCoordinates.y<<"\n";
            }*/
        }

        TraversalData GetLeastComplexPath(TileType *startTile, TileType *endTile, ImprovedGraphType &paths, Array <TileData> &optimalPath)
        {
            auto getPenalty = [] (TileType *tile)
            {
                if(tile->GetTravelPenalty() > 0)
                {
                    return 3;
                }

                return 1;
            };

            paths.Clear();
            auto firstNode = paths.StartGraph({endTile});
            endTile->PathData.IsFreshlyVisited = true;

            TraverseTilesImproved(startTile, endTile, firstNode);

            auto &endNodes = paths.GetEndNodes();
            std::cout<<"possible path count "<<endNodes.GetSize()<<"\n";
            std::cout<<"total node count "<<paths.GetSize()<<"\n";
            for(auto &node : paths.GetNodes())
            {
                //std::cout<<node.Content.Tile->PathData.Passes[0]<<" ";
            }
            std::cout<<"\n";
            std::cout<<"start tile weight "<<startTile->PathData.Passes[0]<<"\n";
            std::cout<<"end tile weight "<<endTile->PathData.Passes[0]<<"\n";

            for(auto &node : endNodes)
            {
                if(node->Content.Tile != startTile)
                    continue;

                auto complexity = 0;
                auto currentNode = node;
                while(true)
                {
                    //std::cout<<currentNode->Content.Tile->PathData.Passes[0]<<" ";
                    complexity += getPenalty(currentNode->Content.Tile);

                    currentNode = currentNode->GetPrevious();
                    if(currentNode == nullptr)
                        break;
                }

                node->Content.Complexity = complexity;
                //std::cout<<"path complexity "<<complexity<<"\n";
            }

            for(auto &node : endNodes)
            {
                auto currentNode = node;
                while(true)
                {
                    std::cout<<currentNode->Content.Tile->PathData.Passes[0]<<" ";

                    currentNode = currentNode->GetPrevious();
                    if(currentNode == nullptr)
                        break;
                }

                //std::cout<<"path complexity "<<node->Content.Complexity<<"\n";
            }

            auto leastComplexity = INT_MAX;
            typename ImprovedGraphType::Node *chosenNode = nullptr;
            for(auto &node : endNodes)
            {
                if(node->Content.Tile != startTile)
                    continue;

                if(node->Content.Complexity < leastComplexity)
                {
                    leastComplexity = node->Content.Complexity;
                    chosenNode = node;
                }
            }

            if(chosenNode != nullptr)
            {
                std::cout<<"chosen complexity "<<chosenNode->Content.Complexity<<"\n\n";
            }
            else
            {
                std::cout<<"no optimal path has been found\n\n";
            }

            optimalPath.Reset();

            if(chosenNode != nullptr)
            {
                while(true)
                {
                    *optimalPath.Add() = {chosenNode->Content.Tile, 0};

                    if(chosenNode->GetPrevious() == nullptr)
                    {
                        break;
                    }

                    chosenNode = chosenNode->GetPrevious();
                }
                std::cout<<"path Length "<<optimalPath.GetSize()<<"\n\n";

                return {true, chosenNode->Content.Complexity};
            }
            else
            {
                return {false, INT_MAX};
            }

            //return visitedTiles;
        }

        const Array <TileData> & FindPathImproved(TileType *startTile, TileType *endTile, Integer range)
        {
            auto &worldTiles = world::WorldScene::Get()->GetTiles();
            for(auto tile = worldTiles.GetStart(); tile != worldTiles.GetEnd(); ++tile)
            {
                tile->PathData.IsVisited = false;
            }

            PassThroughTiles(startTile, range, 0);
            PassThroughTiles(endTile, range, 1);

            auto &tiles = startTile->GetNearbyTiles(range);
            for(auto &tile : tiles)
            {
                tile->PathData.Passes[0] = tile->PathData.Passes[0] + tile->PathData.Passes[1];
                tile->PathData.IsFreshlyVisited = false;
            }

            //return;

            //auto &endTiles = endTile->GetNearbyTiles(range);
            auto firstData = GetLeastComplexPath(startTile, endTile, firstPaths, firstOptimalPath);
            auto secondData = GetLeastComplexPath(endTile, startTile, secondPaths, secondOptimalPath);
            //GetLeastComplexPath(endTile, startTile, secondPaths);

            if(firstData.IsPathValid && secondData.IsPathValid)
            {
                if(firstData.Complexity > secondData.Complexity)
                {
                    return secondOptimalPath;
                }
                else
                {
                    return firstOptimalPath;
                }
            }
            else
            {
                if(firstData.IsPathValid)
                {
                    return firstOptimalPath;
                }
                else if(secondData.IsPathValid)
                {
                    return secondOptimalPath;
                }
                else
                {
                    firstOptimalPath.Reset();
                    return firstOptimalPath;
                }
            }

        }

        void TraverseTilesImproved(TileType *startTile, TileType *endTile, ImprovedGraphType::Node *node)
        {
            /*auto &nodes = improvedPaths.GetNodes();
            for(auto &theNode : nodes)
            {
                //std::cout<<"node "<<theNode.GetLinks().GetSize()<<"\n";
            }*/

            container::SmartBlock <TileType *, 6> neighbours;
            container::SmartBlock <TileType *, 6> contenders;
            neighbours.Clear();
            contenders.Clear();

            auto score = INT_MAX;
            auto nextLowestScore = INT_MAX;
            auto &tiles = node->Content.Tile->GetNearbyTiles();

            bool hasFoundDestination = false;
            for(auto tile = tiles.GetStart(); tile != tiles.GetEnd(); ++tile)
            {
                if(*tile == startTile)
                {
                    hasFoundDestination = true;
                    break;
                }

                if((*tile)->PathData.IsVisited == true && *tile != node->Content.Tile)
                {
                    //std::cout<<(*tile)->PathData.Passes[0]<<" ";

                    *neighbours.Add() = *tile;
                    if(*tile == endTile)
                        continue;

                    if((*tile)->PathData.Passes[0] < score)
                    {
                        nextLowestScore = score;

                        score = (*tile)->PathData.Passes[0];
                        
                    }
                    else if((*tile)->PathData.Passes[0] < nextLowestScore && (*tile)->PathData.Passes[0] > score)
                    {
                        nextLowestScore = (*tile)->PathData.Passes[0];
                    }
                }
            }
            //std::cout<<"       "<<score<<" "<<nextLowestScore<<"\n";

            if(hasFoundDestination == true)
            {
                *contenders.Add() = startTile;
            }
            else
            {
                for(auto tile = neighbours.GetFirst(); tile != neighbours.GetLast(); ++tile)
                {
                    if((*tile)->PathData.Passes[0] == score || (*tile)->PathData.Passes[0] == nextLowestScore)
                    {
                        *contenders.Add() = *tile;
                    }
                }
            }

            for(auto tile = contenders.GetFirst(); tile != contenders.GetLast(); ++tile)
            {
                auto newNode = node->AddNode(*tile);

                if(*tile == startTile)
                    continue;

                auto currentNode = newNode;
                bool hasFoundCircularity = false;
                while(true)
                {
                    currentNode = currentNode->GetPrevious();
                    if(currentNode == nullptr)
                    {
                        break;
                    }
                    else if(currentNode->Content.Tile == newNode->Content.Tile)
                    {
                        hasFoundCircularity = true;
                        break;
                    }
                }

                if(hasFoundCircularity == true)
                    continue;

                TraverseTilesImproved(startTile, endTile, newNode);
            }
        };
    /*
        const Array <TileData> & FindPath(TileType *startTile, TileType *endTile, Integer range)
        {
            auto getPenalty = [] (TileType *tile)
            {
                if(tile->GetTravelPenalty() > 0)
                {
                    return 5;
                }

                return 1;
            };

            visitedTiles.Reset();
            *visitedTiles.Add() = TileData(startTile, getPenalty(startTile));

            fringeTiles.Reset();
            
            auto tileArray = fringeTiles.Add();
            tileArray->Reset();
            *tileArray->Add() = TileData(startTile, getPenalty(startTile));

            for(int k = 1; k <= range; k++)
            {
                fringeTiles.Add()->Reset();
                freshlyVisitedTiles.Reset();

                auto innerFringe = fringeTiles.Get(k - 1);
                for(auto &tile : *innerFringe)
                {
                    auto &neighbours = tile.Tile->GetNearbyTiles(1);
                    for(auto neighbour : neighbours)
                    {
                        //if(neighbour->IsObstacle || neighbour->Combatant)
                        if(neighbour->IsBlocked() == true)
                            continue;

                        bool hasFound = false;
                        for(auto &visitedTile : visitedTiles)
                        {
                            if(visitedTile.Tile == neighbour)
                            {
                                hasFound = true;
                                break;
                            }
                        }

                        if(hasFound == true)
                            continue;

                        for(auto &visitedTile : freshlyVisitedTiles)
                        {
                            if(visitedTile.Tile == neighbour)
                            {
                                hasFound = true;
                                break;
                            }
                        }

                        if(hasFound == true)
                            continue;

                        auto &newNeighbours = neighbour->GetNearbyTiles(1, true);
                        auto lowestScore = INT_MAX;
                        for(auto newNeighbour : newNeighbours)
                        {
                            for(auto &visitedTile : visitedTiles)
                            {
                                if(visitedTile.Tile == newNeighbour)
                                {
                                    if(visitedTile.Distance < lowestScore)
                                    {
                                        lowestScore = visitedTile.Distance;
                                    }

                                    break;
                                }
                            }
                        }

                        auto weight = lowestScore + getPenalty(neighbour);
                        *freshlyVisitedTiles.Add() = {neighbour, weight};

                        auto outerFringe = fringeTiles.Get(k);
                        *outerFringe->Add() = {neighbour, weight};
                    }
                }

                for(auto &tile : freshlyVisitedTiles)
                {
                    *visitedTiles.Add() = tile;
                }
            }

            auto &tiles = world::WorldScene::Get()->GetTiles();
            for(auto tile = tiles.GetStart(); tile != tiles.GetEnd(); ++tile)
            {
                tile->SetPathData(-1);
            }

            for(auto &tile : visitedTiles)
            {
                tile.Tile->SetPathData(tile.Distance);
            }

            auto endTileData = [&] () 
            {
                for(auto &tile : visitedTiles)
                {
                    if(tile.Tile == endTile)
                        return tile;
                }
            } ();

            std::cout<<"start tile complexity "<<getPenalty(startTile)<<"\n";
            std::cout<<"end tile complexity "<<endTileData.Distance<<"\n";

            paths.Clear();
            auto firstNode = paths.StartGraph(endTileData);

            std::cout<<"visited "<<visitedTiles.GetSize()<<"\n";
            TraverseTiles(startTile, endTile, firstNode);

            auto &endNodes = paths.GetEndNodes();
            std::cout<<"possible path count "<<endNodes.GetSize()<<"\n";
            std::cout<<"total node count "<<paths.GetSize()<<"\n";

            for(auto &node : endNodes)
            {
                auto complexity = 0;
                auto currentNode = node;
                while(true)
                {
                    complexity += getPenalty(currentNode->Content.Tile);

                    currentNode = currentNode->GetPrevious();
                    if(currentNode == nullptr)
                        break;
                }

                node->Content.Complexity = complexity;
                std::cout<<"path complexity "<<complexity<<"\n";
            }

            auto leastComplexity = INT_MAX;
            typename GraphType::Node *chosenNode;
            for(auto &node : endNodes)
            {
                if(node->Content.Complexity < leastComplexity)
                {
                    leastComplexity = node->Content.Complexity;
                    chosenNode = node;
                }
            }
            std::cout<<"chosen complexity "<<chosenNode->Content.Complexity<<"\n";

            visitedTiles.Reset();
            while(true)
            {
                *visitedTiles.Add() = chosenNode->Content;

                if(chosenNode->GetPrevious() == nullptr)
                {
                    break;
                }

                chosenNode = chosenNode->GetPrevious();
            }
            std::cout<<"path Length "<<visitedTiles.GetSize()<<"\n\n";

            return visitedTiles;
        }

    private:
        void TraverseTiles(TileType *startTile, TileType *endTile, GraphType::Node *node)
        {
            auto &nodes = paths.GetNodes();
            for(auto &theNode : nodes)
            {
                //std::cout<<"node "<<theNode.GetLinks().GetSize()<<"\n";
            }

            static container::SmartBlock <TileData, 6> neighbours;
            static container::SmartBlock <TileData, 6> contenders;
            neighbours.Clear();
            contenders.Clear();

            auto score = INT_MAX;
            for(auto &tile : visitedTiles)
            {
                if(tile.Tile->GetDistanceTo(*node->Content.Tile) == 1)
                {
                    *neighbours.Add() = tile;
                    if(tile.Distance <= score)
                    {
                        score = tile.Distance;
                    }
                }
            }

            //std::cout<<"score "<<score<<"\n";

            for(auto tileData = neighbours.GetFirst(); tileData != neighbours.GetLast(); ++tileData)
            {
                if(tileData->Distance == score)
                {
                    *contenders.Add() = *tileData;
                }
            }

            for(auto tileData = contenders.GetFirst(); tileData != contenders.GetLast(); ++tileData)
            {
                auto newNode = node->AddNode(*tileData);

                if(tileData->Tile == startTile)
                    continue;

                TraverseTiles(startTile, endTile, newNode);
            }
        };
        */
    };

    //template <class TileType> requires CanBeTravelled <TileType>
    /*template <std::derived_from <core::hex::PhysicalTile> TileType>
    class Hexfinder : public core::Singleton <Hexfinder <TileType>>
    {
        struct TileData : public core::hex::Tile
        {
            TileType *Tile;

            //bool WasVisited;

            Integer Distance;

            TileData(TileType *tile, Integer distance) : Tile(tile), Distance(distance) {}
        };

        container::HexGrid <TileData> tileDatas;

        container::Array <TileData> visitedTiles;

        container::Array <container::Array <TileData>> fringeTiles;

    public:
        Hexfinder()
        {
            tileDatas = container::HexGrid <TileData> (64, 64);
            
            visitedTiles = container::Array <TileData>(1024);

            /*fringeTiles = [] () 
            {
                static auto arrays = Array <Array <TileType *>>(128);

                for(int i = 0; i < arrays.GetCapacity(); ++i)
                {
                    *arrays.Add() = Array <TileType *> (128);
                }

                return arrays;
            } ();
        }

        const Array <TileType> & FindPath(TileType *startTile, TileType *endTile, Integer range)
        {
            visitedTiles.Reset();
            *visitedTiles.Add() = TileData(startTile, 0);

            fringeTiles.Reset();
            
            auto tileArray = fringeTiles.Add();
            tileArray->Reset();
            *tileArray->Add() = startTile;

            for(int k = 1; k <= range; k++)
            {
                fringeTiles.Add()->Reset();
                for(auto &tile : *fringeTiles.Get(k - 1))
                {
                    auto neighbours = tile->GetNearbyTiles(1);
                    for(auto neighbour : neighbours)
                    {
                        //if(neighbour->IsObstacle || neighbour->Combatant)
                        if(neighbour->IsBlocked() == true)
                            continue;

                        bool hasFound = false;
                        for(auto &visitedTile : visitedTiles)
                        {
                            if(visitedTile.Tile == neighbour)
                            {
                                hasFound = true;
                                break;
                            }
                        }

                        if(hasFound == true)
                            continue;

                        *visitedTiles.Add() = {neighbour, k};
                        *fringeTiles.Get(k)->Add() = neighbour;
                    }
                }
            }

            return visitedTiles;
        }
    };*/
}