#pragma once

#include "FlumenCore/Conventions.hpp"

struct BattleTile
{
    Position2 Position;

    Integer2 SquareCoordinates;

    Integer3 HexCoordinates;

    BattleTile() {}

    BattleTile(Position2 position, Integer2 squareCoordinates) : Position(position), SquareCoordinates(squareCoordinates)
    {
        HexCoordinates.x = squareCoordinates.x - squareCoordinates.y / 2;
        HexCoordinates.z = squareCoordinates.y;
        HexCoordinates.y = -HexCoordinates.x - HexCoordinates.z;
    }
};

class BattleMap
{
    friend class BattleScene;

    friend class BattleTileModel;

    Grid <BattleTile> tiles;

    BattleMap() {}

    BattleMap(Length size);

public:
    BattleTile* GetHex(Integer, Integer, Integer);

    BattleTile* GetRandomTile();

    BattleTile* GetCenterTile();
};