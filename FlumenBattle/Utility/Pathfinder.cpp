#include "FlumenBattle/World/WorldTile.h"
#include "Pathfinder.h"

using namespace utility;

WorldPathData::WorldPathData(const typename Pathfinder <world::WorldTile>::PathData &pathData)
{
    for(auto &tile : pathData.Tiles)
    {
        *Tiles.Add() = tile.Tile;
    }

    Complexity = pathData.Complexity;

    Length = pathData.Length;
}