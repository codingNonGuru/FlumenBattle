#pragma once

#include "FlumenBattle/Types.hpp"

namespace world
{
    class WorldTile;
}

namespace world::group
{
    union GroupActionData
    {
        WorldTile *TravelDestination;

        //GroupActionData() {}

        //GroupActionData(WorldTile *travelDestination) : TravelDestination(travelDestination) {}
    };

    struct GroupActionResult
    {
        int Roll;

        int Modifier;

        int DifficultyClass;

        SkillTypes Skill;
    };
}