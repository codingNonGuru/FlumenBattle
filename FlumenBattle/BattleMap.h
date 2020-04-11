#pragma once

#include "FlumenCore/Conventions.hpp"

class Character;
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

    Character* Character;

    BattleTile() {}

    BattleTile(Position2 position, Integer2 squareCoordinates) : Position(position), SquareCoordinates(squareCoordinates), Character(nullptr)
    {
        HexCoordinates.x = squareCoordinates.x - squareCoordinates.y / 2;
        HexCoordinates.z = squareCoordinates.y;
        HexCoordinates.y = -HexCoordinates.x - HexCoordinates.z;
    }

    Integer GetDistanceTo(const BattleTile& other) 
    {
        auto direction = this->HexCoordinates - other.HexCoordinates;
        return (abs(direction.x) + abs(direction.y) + abs(direction.z)) / 2;
    }

    const Array <BattleTile*> & GetNearbyTiles(Integer);
};

class BattleMap
{
    friend class BattleScene;

    friend class BattleTileModel;

    Grid <BattleTile> tiles;

    BattleMap() {}

    BattleMap(Length size);

    BattleTile* GetRandomTile();

public:
    static const Float TILE_DISTANCING = 60.0f;

    BattleTile* GetTile(Integer, Integer, Integer);

    BattleTile* GetEmptyRandomTile();

    BattleTile* GetCenterTile();

    const Array<BattleTile*> & GetNearbyTiles(BattleTile*, Integer);
};