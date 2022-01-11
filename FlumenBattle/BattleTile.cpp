#include "FlumenEngine/Utility/Color.hpp"

#include "FlumenBattle/BattleTile.h"
#include "FlumenBattle/BattleMap.h"

BattleTile::BattleTile(Position2 position, Integer2 squareCoordinates) : Position(position), SquareCoordinates(squareCoordinates), Combatant(nullptr)
{
    HexCoordinates.x = squareCoordinates.x - squareCoordinates.y / 2;
    HexCoordinates.z = squareCoordinates.y;
    HexCoordinates.y = -HexCoordinates.x - HexCoordinates.z;

    Color color(0.8f, 0.6f, 0.5f, 1.0f);
    color = Color::AddSaturation(color, utility::GetRandom(-0.1f, 0.1f));
    color = Color::Lighten(color, utility::GetRandom(-0.1f, 0.1f));

    Shade = color;
}

const Array <BattleTile*> & BattleTile::GetNearbyTiles(Integer range)
{
    return Map->GetNearbyTiles(this, range);
}

BattleTile* BattleTile::GetEmptyTileInRange(Integer range)
{
    return Map->GetEmptyTileAroundTile(this, range);
}

BattleTile * BattleTile::GetNeighbor(Integer3 direction)
{
    auto coordinates = HexCoordinates + direction;
    return Map->GetTile(coordinates);
}