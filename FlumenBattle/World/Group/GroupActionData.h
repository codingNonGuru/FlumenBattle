#pragma once

#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/Utility/Utility.h"

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
        bool HasRolled;

        utility::Success Success;

        SkillTypes Skill;

        GroupActionResult(utility::Success success, SkillTypes skill) : 
        HasRolled(true), Success(success), Skill(skill) {}

        GroupActionResult() : HasRolled(false) {}
    };
}