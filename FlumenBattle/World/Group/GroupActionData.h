#pragma once

#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/World/Character/Types.h"
#include "FlumenBattle/World/Group/Types.h"
#include "FlumenBattle/Utility/Utility.h"

namespace world
{
    class WorldTile;

    namespace settlement
    {
        class Settlement;
    }
}

namespace world::group
{
    class Group;

    union GroupActionData
    {
        WorldTile *TravelDestination;

        settlement::Settlement *LootTarget;

        bool IsEngaged {false};

        GroupActionData() {}

        GroupActionData(WorldTile *travelDestination) : TravelDestination(travelDestination) {}

        GroupActionData(settlement::Settlement *lootTarget) : LootTarget(lootTarget) {}

        GroupActionData(bool isEngaged) : IsEngaged(isEngaged) {}
    };

    struct GroupActionResult
    {
        #define STRONGLY_TYPED_INTEGER(Type) struct Type {int value; operator int() {return value;}}

        STRONGLY_TYPED_INTEGER(Food);

        STRONGLY_TYPED_INTEGER(Money);

        GroupActions ActionType;

        bool HasRolled;

        utility::Success Success;

        character::SkillTypes Skill;

        union SpecificContent
        {
            Group *spottedGroup;

            Food foragedFood;

            bool isAlreadyEngaged;

            Money lootedMoney;

            SpecificContent() {}

            SpecificContent(Group *group) : spottedGroup(group) {}

            explicit SpecificContent(Food food) : foragedFood(food) {}

            SpecificContent(bool isEngaged) : isAlreadyEngaged(isEngaged) {}

            explicit SpecificContent(Money money) : lootedMoney(money) {}
        } Content;

        GroupActionResult(GroupActions actionType, utility::Success success, character::SkillTypes skill) : 
        ActionType(actionType), HasRolled(true), Success(success), Skill(skill) {}

        GroupActionResult(GroupActions actionType, utility::Success success, character::SkillTypes skill, Group *spottedGroup) : 
        ActionType(actionType), HasRolled(true), Success(success), Skill(skill), Content{spottedGroup} {}

        GroupActionResult(GroupActions actionType, utility::Success success, character::SkillTypes skill, Food foragedFood) : 
        ActionType(actionType), HasRolled(true), Success(success), Skill(skill), Content{foragedFood} {}

        GroupActionResult(GroupActions actionType, utility::Success success, character::SkillTypes skill, bool isEngaged) : 
        ActionType(actionType), HasRolled(true), Success(success), Skill(skill), Content{isEngaged} {}

        GroupActionResult(GroupActions actionType, Money money) : 
        ActionType(actionType), HasRolled(false), Content{money} {}

        GroupActionResult() : HasRolled(false) {}

        GroupActionResult(GroupActions actionType) : ActionType(actionType), HasRolled(false) {}
    };
}