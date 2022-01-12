#pragma once

#include "FlumenCore/Conventions.hpp"

namespace world
{
    class WorldMap;
    namespace group
    {
        class Group;
    }
}

namespace world
{
    struct WorldTile
    {
        friend class world::WorldMap;

        friend class world::group::Group;

    private:
        WorldMap *map;

        group::Group *group;

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

        void Initialize();

        Integer GetDistanceTo(const WorldTile& other) 
        {
            auto direction = this->HexCoordinates - other.HexCoordinates;
            return (abs(direction.x) + abs(direction.y) + abs(direction.z)) / 2;
        }

        const Array <WorldTile*> & GetNearbyTiles(Integer);

        WorldTile * GetEmptyTileInRange(Integer);

        WorldTile * GetNeighbor(Integer3);

        group::Group * GetGroup() {return group;}
    };
}