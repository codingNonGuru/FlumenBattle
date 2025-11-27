#pragma once

#include "FlumenCore/Conventions.hpp"
#include "FlumenCore/Hex/Hex.h"

#include "FlumenEngine/Thread/HexBuffer.h"

#include "FlumenBattle/Types.hpp"

namespace battle
{
    class Combatant;
    class BattleMap;
}

namespace world
{
    namespace tile {class WorldMap;}
}

namespace battle
{
    struct BattleTile : public core::hex::PhysicalTile
    {
        friend class BattleMap;

        friend class world::tile::WorldMap;

    private:
        BattleMap* Map;

    public:
        struct OldPathData
        {
            BattleTile *Tile;

            Integer Distance;
        };

        Integer2 SquareCoordinates;

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

        BattleTile(Position2 position, Integer2 squareCoordinates);

        int GetPenalty() const;

        TravelPenalty GetTravelPenalty() const;

        const engine::thread::HexBuffer <BattleTile> GetNearbyTiles(Integer);

        container::Block <BattleTile *, 6> GetNearbyTiles();

        BattleTile * GetEmptyTileInRange(Integer);

        BattleTile * GetNeighbor(Integer3);

        const engine::thread::HexBuffer <BattleTile> GetTileRing(Integer);

        const Array <OldPathData> & FindPath(Integer);
    };
}