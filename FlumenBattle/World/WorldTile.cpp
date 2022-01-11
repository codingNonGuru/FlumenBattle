#include "FlumenEngine/Utility/Color.hpp"

#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/WorldMap.h"

using namespace world;

WorldTile::WorldTile(Position2 position, Integer2 squareCoordinates) : Position(position), SquareCoordinates(squareCoordinates)
{
    HexCoordinates.x = squareCoordinates.x - squareCoordinates.y / 2;
    HexCoordinates.z = squareCoordinates.y;
    HexCoordinates.y = -HexCoordinates.x - HexCoordinates.z;

    Color color(0.4f, 0.6f, 0.1f, 1.0f);
    color = Color::AddSaturation(color, utility::GetRandom(-0.1f, 0.1f));
    color = Color::Lighten(color, utility::GetRandom(-0.1f, 0.1f));

    Shade = color;
}

const Array <WorldTile*> & WorldTile::GetNearbyTiles(Integer range)
{
    return Map->GetNearbyTiles(this, range);
}

WorldTile* WorldTile::GetEmptyTileInRange(Integer range)
{
    return Map->GetEmptyTileAroundTile(this, range);
}

WorldTile * WorldTile::GetNeighbor(Integer3 direction)
{
    auto coordinates = HexCoordinates + direction;
    return Map->GetTile(coordinates);
}