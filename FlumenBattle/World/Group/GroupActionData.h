#pragma once

#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/World/Character/Types.h"
#include "FlumenBattle/World/Group/Types.h"
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
        GroupActions ActionType;

        bool HasRolled;

        utility::Success Success;

        character::SkillTypes Skill;

        union SpecificContent
        {
            Group *spottedGroup;

            SpecificContent() {}

            SpecificContent(Group *group) : spottedGroup(group) {}
        } Content;

        GroupActionResult(GroupActions actionType, utility::Success success, character::SkillTypes skill) : 
        ActionType(actionType), HasRolled(true), Success(success), Skill(skill) {}

        GroupActionResult(GroupActions actionType, utility::Success success, character::SkillTypes skill, Group *spottedGroup) : 
        ActionType(actionType), HasRolled(true), Success(success), Skill(skill), Content{spottedGroup} {}

        GroupActionResult() : HasRolled(false) {}

        GroupActionResult(GroupActions actionType) : ActionType(actionType), HasRolled(false) {}
    };
}