#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/Battle/BattleTile.h"
#include "Pathfinder.h"
#include "FlumenBattle/World/WorldScene.h"

using namespace utility;

template <class TileType>
PathData <TileType>::PathData(GenericPathData *genericPathData)
{
    if constexpr(std::same_as <TileType, world::WorldTile>)
    {
        auto pathData = static_cast <utility::PathfindResult <world::WorldTile> *> (genericPathData);
        for(auto &tile : pathData->Tiles)
        {
            *Tiles.Add() = tile.Tile;
        }

        Complexity = pathData->Complexity;

        Length = pathData->Length;
    }
    else
    {
        auto pathData = static_cast <utility::PathfindResult <battle::BattleTile> *> (genericPathData);
        for(auto &tile : pathData->Tiles)
        {
            *Tiles.Add() = tile.Tile;
        }

        Complexity = pathData->Complexity;

        Length = pathData->Length;
    }
}

template class utility::PathData <world::WorldTile>;
template class utility::PathData <battle::BattleTile>;

template <class TileType> requires CanBeTravelled <TileType>
Pathfinder <TileType>::Pathfinder()
{
    visitedTiles = Array <TileData <TileType>>(4096);

    tilePaths = TileGraph(4096);

    auto map = world::WorldScene::Get()->GetWorldMap();
    nodeMap.Initialize(map->GetSize(), map->GetSize());

    auto &worldTiles = map->GetTiles();
    for(auto tile = worldTiles.GetStart(); tile != worldTiles.GetEnd(); ++tile)
    {
        nodeMap.GetTile(tile->HexCoordinates)->Coordinates = tile->HexCoordinates;
    }
}

template class utility::Pathfinder <world::WorldTile>;
template class utility::Pathfinder <battle::BattleTile>;