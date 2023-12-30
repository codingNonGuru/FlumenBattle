#pragma once

#include <concepts>

#include "FlumenCore/Singleton.h"
#include "FlumenCore/Container/HexGrid.h"
#include "FlumenCore/Container/Graph.h"

#include "FlumenBattle/World/WorldScene.h"

namespace utility
{
    template<typename T>
	concept CanBeTravelled = requires(T a, int b)
	{
        { a.GetNearbyTiles(b) };

        { a.IsBlocked() };

        { a.GetTravelPenalty() };
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

            TileData(TileType *tile, Integer distance) : Tile(tile), Distance(distance), IsFringe(false) {}
        };

        typedef container::Graph <TileData, 6> GraphType;
        typedef container::Graph <TileType *, 6> ImprovedGraphType;

        Array <TileData> visitedTiles;

        Array <TileData> freshlyVisitedTiles;

        Array <Array <TileData>> fringeTiles;

        //Array <TileType *> 

        GraphType paths;

        ImprovedGraphType improvedPaths;

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

            paths = GraphType(1024);

            improvedPaths = ImprovedGraphType(1024);
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

        const Array <TileData> & FindPathImproved(TileType *startTile, TileType *endTile, Integer range)
        {
            auto getPenalty = [] (TileType *tile)
            {
                if(tile->GetTravelPenalty() > 0)
                {
                    return 5;
                }

                return 1;
            };

            PassThroughTiles(startTile, 10, 0);
            //PassThroughTiles(endTile, 10, 1);

            auto &tiles = startTile->GetNearbyTiles(range);
            for(auto &tile : tiles)
            {
                //tile->PathData.Passes[0] = tile->PathData.Passes[0] + tile->PathData.Passes[1];
            }

            //auto &endTiles = endTile->GetNearbyTiles(range);
            improvedPaths.Clear();
            auto firstNode = improvedPaths.StartGraph(endTile);

            TraverseTilesImproved(startTile, endTile, firstNode);

            auto &endNodes = improvedPaths.GetEndNodes();
            for(auto &node : endNodes)
            {
                auto complexity = 0;
                auto currentNode = node;
                while(true)
                {
                    complexity += getPenalty(currentNode->Content);

                    currentNode = currentNode->GetPrevious();
                    if(currentNode == nullptr)
                        break;
                }

                node->Content->PathData.Passes[1] = complexity;
                std::cout<<"path complexity "<<complexity<<"\n";
            }

            auto leastComplexity = INT_MAX;
            typename ImprovedGraphType::Node *chosenNode;
            for(auto &node : endNodes)
            {
                if(node->Content->PathData.Passes[1] < leastComplexity)
                {
                    leastComplexity = node->Content->PathData.Passes[1];
                    chosenNode = node;
                }
            }
            std::cout<<"chosen complexity "<<chosenNode->Content->PathData.Passes[1]<<"\n";

            visitedTiles.Reset();
            while(true)
            {
                *visitedTiles.Add() = {chosenNode->Content, 0};

                if(chosenNode->GetPrevious() == nullptr)
                {
                    break;
                }

                chosenNode = chosenNode->GetPrevious();
            }
            std::cout<<"path Length "<<visitedTiles.GetSize()<<"\n\n";

            return visitedTiles;
        }

        void TraverseTilesImproved(TileType *startTile, TileType *endTile, ImprovedGraphType::Node *node)
        {
            /*auto &nodes = improvedPaths.GetNodes();
            for(auto &theNode : nodes)
            {
                //std::cout<<"node "<<theNode.GetLinks().GetSize()<<"\n";
            }*/

            static container::SmartBlock <TileType *, 6> neighbours;
            static container::SmartBlock <TileType *, 6> contenders;
            neighbours.Clear();
            contenders.Clear();

            auto score = INT_MAX;
            auto &tiles = node->Content->GetNearbyTiles(1);
            for(auto &tile : tiles)
            {
                if(tile->PathData.IsVisited == true && tile != node->Content)
                {
                    *neighbours.Add() = tile;
                    if(tile->PathData.Passes[0] <= score)
                    {
                        score = tile->PathData.Passes[0];
                    }
                }
            }

            for(auto tile = neighbours.GetFirst(); tile != neighbours.GetLast(); ++tile)
            {
                if((*tile)->PathData.Passes[0] == score)
                {
                    *contenders.Add() = *tile;
                }
            }

            for(auto tile = contenders.GetFirst(); tile != contenders.GetLast(); ++tile)
            {
                auto newNode = node->AddNode(*tile);

                if(*tile == startTile)
                    continue;

                TraverseTilesImproved(startTile, endTile, newNode);
            }
        };

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