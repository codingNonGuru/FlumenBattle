#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenBattle/Types.hpp"

namespace battle
{
    class Combatant;
    class BattleMap;
}

namespace world
{
    class WorldMap;
}

namespace battle
{
    struct BattleTile
    {
        friend class BattleMap;

        friend class world::WorldMap;

    private:
        BattleMap* Map;

    public:
        struct OldPathData
        {
            BattleTile *Tile;

            Integer Distance;
        };

        Position2 Position;

        Integer2 SquareCoordinates;

        Integer3 HexCoordinates;

        Float4 Shade;

        class Combatant *Combatant;

        bool IsObstacle;

        struct PathData
        {
            bool IsVisited = false;

            bool IsTotallyVisited;

            bool IsToBeVisited = false;
        } PathData;

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

        int GetPenalty() const;

        TravelPenalty GetTravelPenalty() const;

        const Array <BattleTile *> &GetNearbyTiles(Integer);

        container::Block <BattleTile *, 6> GetNearbyTiles();

        BattleTile * GetEmptyTileInRange(Integer);

        BattleTile * GetNeighbor(Integer3);

        const Array <BattleTile *> &GetTileRing(Integer);

        const Array <OldPathData> & FindPath(Integer);
    };
}