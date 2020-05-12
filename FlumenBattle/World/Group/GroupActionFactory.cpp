#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenBattle/World/Group/GroupActionFactory.h"
#include "FlumenBattle/World/Group/GroupAction.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/Character.h"

namespace world::group
{
    const GroupAction * GroupActionFactory::BuildAction(GroupActions actionType)
    {
        switch(actionType)
        {
            case GroupActions::TAKE_SHORT_REST:
                return BuildTakeShortRest();
            case GroupActions::TAKE_LONG_REST:
                return BuildTakeLongRest();
            case GroupActions::SEARCH:
                return BuildSearch();
            case GroupActions::FIGHT:
                return BuildFight();
        }
    }

    const GroupAction * GroupActionFactory::BuildTakeShortRest()
    {
        static GroupAction action = {GroupActions::TAKE_SHORT_REST, 1, &GroupActionValidator::CanTakeShortRest, &GroupActionPerformer::TakeShortRest};
        return &action;
    }

    const GroupAction * GroupActionFactory::BuildTakeLongRest()
    {
        static GroupAction action = {GroupActions::TAKE_LONG_REST, 8, &GroupActionValidator::CanTakeLongRest, &GroupActionPerformer::TakeLongRest};
        return &action;
    }

    const GroupAction * GroupActionFactory::BuildSearch()
    {
        static GroupAction action = {GroupActions::SEARCH, 3, &GroupActionValidator::CanSearch, &GroupActionPerformer::Search};
        return &action;
    }

    const GroupAction * GroupActionFactory::BuildFight()
    {
        static GroupAction action = {GroupActions::FIGHT, 0, &GroupActionValidator::CanFight, &GroupActionPerformer::Fight};
        return &action;
    }

    void GroupActionPerformer::TakeShortRest(Group& group)
    {
        if(group.actionProgress != group.action->Duration)
            return;
        
        auto &characters = group.GetCharacters();
        for(auto character = characters.GetStart(); character != characters.GetEnd(); ++character)
        {
            character->TakeShortRest();
        }

        group.CancelAction();
    }

    void GroupActionPerformer::TakeLongRest(Group& group)
    {
        if(group.actionProgress != group.action->Duration)
            return;

        auto &characters = group.GetCharacters();
        for(auto character = characters.GetStart(); character != characters.GetEnd(); ++character)
        {
            character->TakeLongRest();
        }

        group.CancelAction();
    }

    void GroupActionPerformer::Search(Group& group)
    {
        if(group.actionProgress < group.action->Duration)
            return;

        auto &groups = WorldScene::Get()->GetGroups();
        auto other = groups.GetRandom();

        if(other == &group)
            return;

        if(other->GetAction() && other->GetAction()->Type == GroupActions::FIGHT)
            return;

        auto perceptionBonus = -50;
        for(auto &character : group.characters)
        {
            auto bonus = character.GetPerceptionProficiencyBonus();
            if(bonus > perceptionBonus)
                perceptionBonus = bonus;
        }

        auto stealthBonus = 50;
        for(auto &character : group.characters)
        {
            auto bonus = character.GetAbility(AbilityTypes::DEXTERITY).Modifier;
            if(bonus < stealthBonus)
                stealthBonus = bonus;
        }

        auto perceptionCheck = utility::GetRandom(1, 20) + perceptionBonus - stealthBonus;
        if(perceptionCheck <= 20)
            return;

        WorldScene::Get()->StartBattle(&group, other);
    }

    void GroupActionPerformer::Fight(Group& group)
    {
    }

    bool GroupActionValidator::CanTakeShortRest(Group &group)
    {
        return true;
    }

    bool GroupActionValidator::CanTakeLongRest(Group &group)
    {
        return true;
    }

    bool GroupActionValidator::CanSearch(Group &group)
    {
        return true;
    }

    bool GroupActionValidator::CanFight(Group &group)
    {
        return true;
    }
}