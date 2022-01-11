#pragma once

#include "FlumenCore/Conventions.hpp"

class Combatant;
namespace world
{
    class WorldMap;
}

namespace world
{
    struct WorldTile
    {
        friend class world::WorldMap;

    private:
        WorldMap* Map;

    public:
        Position2 Position;

        Integer2 SquareCoordinates;

        Integer3 HexCoordinates;

        Float4 Shade;

        WorldTile() {}

        WorldTile(Position2 position, Integer2 squareCoordinates);/* : Position(position), SquareCoordinates(squareCoordinates), Combatant(nullptr)
        {
            HexCoordinates.x = squareCoordinates.x - squareCoordinates.y / 2;
            HexCoordinates.z = squareCoordinates.y;
            HexCoordinates.y = -HexCoordinates.x - HexCoordinates.z;
        }*/

        Integer GetDistanceTo(const WorldTile& other) 
        {
            auto direction = this->HexCoordinates - other.HexCoordinates;
            return (abs(direction.x) + abs(direction.y) + abs(direction.z)) / 2;
        }

        const Array <WorldTile*> & GetNearbyTiles(Integer);

        WorldTile * GetEmptyTileInRange(Integer);

        WorldTile * GetNeighbor(Integer3);
    };
}