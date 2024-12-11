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

        bool IsEngaged {false};

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

            int foragedFood;

            bool isAlreadyEngaged;

            SpecificContent() {}

            SpecificContent(Group *group) : spottedGroup(group) {}

            SpecificContent(int food) : foragedFood(food) {}

            SpecificContent(bool isEngaged) : isAlreadyEngaged(isEngaged) {}
        } Content;

        GroupActionResult(GroupActions actionType, utility::Success success, character::SkillTypes skill) : 
        ActionType(actionType), HasRolled(true), Success(success), Skill(skill) {}

        GroupActionResult(GroupActions actionType, utility::Success success, character::SkillTypes skill, Group *spottedGroup) : 
        ActionType(actionType), HasRolled(true), Success(success), Skill(skill), Content{spottedGroup} {}

        GroupActionResult(GroupActions actionType, utility::Success success, character::SkillTypes skill, int foragedFood) : 
        ActionType(actionType), HasRolled(true), Success(success), Skill(skill), Content{foragedFood} {}

        GroupActionResult(GroupActions actionType, utility::Success success, character::SkillTypes skill, bool isEngaged) : 
        ActionType(actionType), HasRolled(true), Success(success), Skill(skill), Content{isEngaged} {}

        GroupActionResult() : HasRolled(false) {}

        GroupActionResult(GroupActions actionType) : ActionType(actionType), HasRolled(false) {}
    };
}