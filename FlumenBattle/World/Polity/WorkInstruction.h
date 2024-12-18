#pragma once

#include "FlumenCore/Container/Pool.hpp"

namespace world::settlement
{
    class Settlement;
    class Building;
    struct SettlementTile;
}

namespace world::polity
{
    struct WorkInstruction
    {
        union Place
        {
            settlement::Building *Building;

            settlement::SettlementTile *Tile;

            Place() {}

            Place(settlement::Building *building) : Building(building) {}

            Place(settlement::SettlementTile *tile) : Tile(tile) {}
        };

        int Priority;

        enum {BUILDING, TILE} PlaceType;

        Place Place;

        bool operator ==(const settlement::Building *building) {return Place.Building == building;}

        bool operator ==(const settlement::SettlementTile *tile) {return Place.Tile == tile;}
    };

    struct InstructionSet
    {
        settlement::Settlement *settlement;

        container::Pool <WorkInstruction> instructions;

        bool operator ==(const settlement::Settlement *other) {return settlement == other;}
    };
}