#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenBattle/World/Group/GroupActionFactory.h"
#include "FlumenBattle/World/Group/GroupAction.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/WorldTile.h"
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
            case GroupActions::TRAVEL:
                return BuildTravel();
        }
    }

    const GroupAction * GroupActionFactory::BuildTakeShortRest()
    {
        static GroupAction action = {GroupActions::TAKE_SHORT_REST, 18, &GroupActionValidator::CanTakeShortRest, &GroupActionPerformer::TakeShortRest};
        return &action;
    }

    const GroupAction * GroupActionFactory::BuildTakeLongRest()
    {
        static GroupAction action = {GroupActions::TAKE_LONG_REST, 48, &GroupActionValidator::CanTakeLongRest, &GroupActionPerformer::TakeLongRest};
        return &action;
    }

    const GroupAction * GroupActionFactory::BuildSearch()
    {
        static GroupAction action = {GroupActions::SEARCH, 18, &GroupActionValidator::CanSearch, &GroupActionPerformer::Search};
        return &action;
    }

    const GroupAction * GroupActionFactory::BuildFight()
    {
        static GroupAction action = {GroupActions::FIGHT, 0, &GroupActionValidator::CanFight, &GroupActionPerformer::Fight};
        return &action;
    }

    const GroupAction * GroupActionFactory::BuildTravel()
    {
        static GroupAction action = {
            GroupActions::TRAVEL, 
            60, 
            &GroupActionValidator::CanTravel, 
            &GroupActionPerformer::Travel, 
            &GroupActionPerformer::InitiateTravel, 
            &GroupActionPerformer::GetTravelDuration
            };
        return &action;
    }

    void GroupActionPerformer::InitiateTravel(Group &group, const GroupActionData &actionData)
    {
        group.destination = actionData.TravelDestination;

        auto difficultyClass = 15;

        int modifier = -100;
        for(auto &character : group.characters)
        {
            if(character.GetSkillProficiency(SkillTypes::SURVIVAL) > modifier)
                modifier = character.GetSkillProficiency(SkillTypes::SURVIVAL);
        }

        auto roll = utility::GetRandom(1, 20);
        if(roll == 1 || roll + modifier <= difficultyClass - 10)
        {
            group.actionSuccess = SuccessTypes::CRITICAL_FAILURE;
        }
        else if(roll == 20 || roll + modifier > difficultyClass + 10)
        {
            group.actionSuccess = SuccessTypes::CRITICAL_SUCCESS;
        }
        else if(roll + modifier <= difficultyClass)
        {
            group.actionSuccess = SuccessTypes::FAILURE;
        }
        else if(roll + modifier > difficultyClass)
        {
            group.actionSuccess = SuccessTypes::SUCCESS;
        }
    }

    int GroupActionPerformer::GetTravelDuration(const Group &group)
    {
        int durationBonus = 0;
        switch(group.actionSuccess)
        {
            case SuccessTypes::CRITICAL_SUCCESS:
                durationBonus = 4;
                break;
            case SuccessTypes::SUCCESS:
                durationBonus = 2;
                break;
            case SuccessTypes::CRITICAL_FAILURE:
                durationBonus = -2;
                break;
        }

        return group.action->BaseDuration - durationBonus * 6;
    }

    void GroupActionPerformer::TakeShortRest(Group& group)
    {
        if(group.actionProgress != group.action->BaseDuration)
            return;
        
        auto &characters = group.GetCharacters();
        for(auto &character : characters)
        {
            character.TakeShortRest();
        }

        group.CancelAction();
    }

    void GroupActionPerformer::TakeLongRest(Group& group)
    {
        if(group.actionProgress != group.action->BaseDuration)
            return;

        auto &characters = group.GetCharacters();
        for(auto &character : group.characters)
        {
            character.TakeLongRest();
        }

        group.CancelAction();
    }

    void GroupActionPerformer::Search(Group& group)
    {
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

        if(group.actionProgress < group.action->BaseDuration)
            return;

        group.CancelAction();

        //WorldScene::Get()->StartBattle(&group, other);
    }

    void GroupActionPerformer::Fight(Group& group)
    {
        
    }

    void GroupActionPerformer::Travel(Group& group)
    {
        if(group.actionProgress < group.action->GetDuration(group))
            return;

        group.SetTile(group.destination);
        group.destination = nullptr;

        group.CancelAction();
    }

    bool GroupActionValidator::CanTakeShortRest(Group &group, const GroupActionData &)
    {
        return true;
    }

    bool GroupActionValidator::CanTakeLongRest(Group &group, const GroupActionData &)
    {
        return true;
    }

    bool GroupActionValidator::CanSearch(Group &group, const GroupActionData &)
    {
        return true;
    }

    bool GroupActionValidator::CanFight(Group &group, const GroupActionData &)
    {
        return true;
    }

    bool GroupActionValidator::CanTravel(Group &group, const GroupActionData &data)
    {
        if(data.TravelDestination == nullptr)
            return false;

        if(data.TravelDestination->Type == WorldTiles::SEA || data.TravelDestination->GetGroup() != nullptr)
            return false;

        if(data.TravelDestination->GetDistanceTo(*group.tile) != 1)
            return false;
            
        return true;
    }
}