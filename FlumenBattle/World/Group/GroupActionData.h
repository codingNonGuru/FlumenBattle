#pragma once

#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/World/Character/Types.h"
#include "FlumenBattle/World/Group/Types.h"
#include "FlumenBattle/Utility/Utility.h"

namespace world
{
    namespace tile {struct WorldTile;}

    namespace settlement
    {
        class Settlement;
    }
}

namespace world::group
{
    class GroupCore;

    union GroupActionData
    {
        tile::WorldTile *TravelDestination;

        settlement::Settlement *LootTarget;

        bool IsEngaged {false};

        GroupActionData() {}

        GroupActionData(tile::WorldTile *travelDestination) : TravelDestination(travelDestination) {}

        GroupActionData(settlement::Settlement *lootTarget) : LootTarget(lootTarget) {}

        GroupActionData(bool isEngaged) : IsEngaged(isEngaged) {}
    };

    struct GroupActionResult
    {
        #define STRONGLY_TYPED_INTEGER(Type) struct Type {int value; operator int() {return value;}}

        STRONGLY_TYPED_INTEGER(Food);

        STRONGLY_TYPED_INTEGER(Money);

        struct LootData {Food Food; Money Money;};

        GroupActions ActionType;

        bool HasRolled;

        utility::Success Success;

        character::SkillTypes Skill;

        union SpecificContent
        {
            GroupCore *spottedGroup;

            Food foragedFood;

            bool isAlreadyEngaged;

            LootData lootedStuff;

            Money bribeMoney;

            SpecificContent() {}

            SpecificContent(GroupCore *group) : spottedGroup(group) {}

            SpecificContent(Food food) : foragedFood(food) {}

            SpecificContent(bool isEngaged) : isAlreadyEngaged(isEngaged) {}

            SpecificContent(Food food, Money money) : lootedStuff{food, money} {}

            SpecificContent(Money money) : bribeMoney{money} {}
        } Content;

        GroupActionResult(GroupActions actionType, utility::Success success, character::SkillTypes skill) : 
        ActionType(actionType), HasRolled(true), Success(success), Skill(skill) {}

        GroupActionResult(GroupActions actionType, utility::Success success, character::SkillTypes skill, GroupCore *spottedGroup) : 
        ActionType(actionType), HasRolled(true), Success(success), Skill(skill), Content{spottedGroup} {}

        GroupActionResult(GroupActions actionType, utility::Success success, character::SkillTypes skill, Food foragedFood) : 
        ActionType(actionType), HasRolled(true), Success(success), Skill(skill), Content{foragedFood} {}

        GroupActionResult(GroupActions actionType, utility::Success success, character::SkillTypes skill, bool isEngaged) : 
        ActionType(actionType), HasRolled(true), Success(success), Skill(skill), Content{isEngaged} {}

        GroupActionResult(GroupActions actionType, utility::Success success, character::SkillTypes skill, Food food, Money money) : 
        ActionType(actionType), HasRolled(true), Success(success), Skill(skill), Content{food, money} {}

        GroupActionResult(GroupActions actionType, utility::Success success, character::SkillTypes skill, Money money) : 
        ActionType(actionType), HasRolled(true), Success(success), Skill(skill), Content{money} {}

        GroupActionResult() : HasRolled(false) {}

        GroupActionResult(GroupActions actionType) : ActionType(actionType), HasRolled(false) {}
    };
}