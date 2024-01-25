#pragma once

#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/Utility/Utility.h"

namespace world
{
    class WorldTile;
}

namespace world::group
{
    class Group;

    union GroupActionData
    {
        WorldTile *TravelDestination;

        bool IsEngaged;

        GroupActionData() {}

        GroupActionData(WorldTile *travelDestination) : TravelDestination(travelDestination) {}

        GroupActionData(bool isEngaged) : IsEngaged(isEngaged) {}
    };

    struct GroupActionResult
    {
        bool HasRolled;

        utility::Success Success;

        SkillTypes Skill;

        union SpecificContent
        {
            Group *spottedGroup;
        } Content;

        GroupActionResult(utility::Success success, SkillTypes skill) : 
        HasRolled(true), Success(success), Skill(skill) {}

        GroupActionResult(utility::Success success, SkillTypes skill, Group *spottedGroup) : 
        HasRolled(true), Success(success), Skill(skill), Content{spottedGroup} {}

        GroupActionResult() : HasRolled(false) {}
    };
}