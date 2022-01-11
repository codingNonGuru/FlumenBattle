#pragma once

#include "FlumenCore/Conventions.hpp"

class Combatant;
class BattleMap;

struct BattleTile
{
    friend class BattleMap;

private:
    BattleMap* Map;

public:
    Position2 Position;

    Integer2 SquareCoordinates;

    Integer3 HexCoordinates;

    Float4 Shade;

    class Combatant *Combatant;

    BattleTile() {}

    BattleTile(Position2 position, Integer2 squareCoordinates);/* : Position(position), SquareCoordinates(squareCoordinates), Combatant(nullptr)
    {
        HexCoordinates.x = squareCoordinates.x - squareCoordinates.y / 2;
        HexCoordinates.z = squareCoordinates.y;
        HexCoordinates.y = -HexCoordinates.x - HexCoordinates.z;
    }*/

    Integer GetDistanceTo(const BattleTile& other) 
    {
        auto direction = this->HexCoordinates - other.HexCoordinates;
        return (abs(direction.x) + abs(direction.y) + abs(direction.z)) / 2;
    }

    const Array <BattleTile*> & GetNearbyTiles(Integer);

    BattleTile * GetEmptyTileInRange(Integer);

    BattleTile * GetNeighbor(Integer3);
};