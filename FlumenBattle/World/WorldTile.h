#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenBattle/World/Types.h"

namespace world
{
    class WorldMap;
    struct WorldBiome;
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

        WorldTiles Type;

        const WorldBiome *Biome;

        WorldTile() {}

        WorldTile(Position2 position, Integer2 squareCoordinates);

        void Initialize();

        Integer GetDistanceTo(const WorldTile& other) 
        {
            auto direction = this->HexCoordinates - other.HexCoordinates;
            return (abs(direction.x) + abs(direction.y) + abs(direction.z)) / 2;
        }

        Float GetPhysicalDistanceTo(const WorldTile& other)
        {
            auto direction = this->Position - other.Position;
            return glm::length(direction);
        }

        const Array <WorldTile*> & GetNearbyTiles(Integer);

        WorldTile * GetEmptyTileInRange(Integer);

        WorldTile * GetNeighbor(Integer3);

        group::Group * GetGroup() {return group;}
    };
}