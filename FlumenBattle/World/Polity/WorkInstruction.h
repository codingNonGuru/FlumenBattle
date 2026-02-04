#pragma once

#include "FlumenCore/Container/Pool.hpp"

namespace world::settlement
{
    class Settlement;
    class Building;
    struct SettlementTile;
    class Job;
    struct Resource;
}

namespace world::polity
{
    struct WorkInstruction
    {
        union Place
        {
            settlement::Resource *Resource;

            settlement::SettlementTile *Tile;

            Place() {}

            Place(settlement::Resource *resource) : Resource(resource) {}

            Place(settlement::SettlementTile *tile) : Tile(tile) {}
        };

        int Priority;

        enum {RESOURCE, TILE} PlaceType;

        Place Place;

        //settlement::Job *Job;

        bool operator ==(const settlement::Resource *resource) {return Place.Resource == resource;}

        bool operator ==(const settlement::SettlementTile *tile) {return Place.Tile == tile;}

        bool operator ==(const int priority) {return Priority == priority;}
    };

    struct InstructionSet
    {
        settlement::Settlement *settlement;

        container::Pool <WorkInstruction> instructions;

        bool operator ==(const settlement::Settlement *other) {return settlement == other;}
    };
}