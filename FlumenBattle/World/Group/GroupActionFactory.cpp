#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenBattle/World/Group/GroupActionFactory.h"
#include "FlumenBattle/World/Group/GroupAction.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/WorldBiome.h"
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
            case GroupActions::ENGAGE:
                return BuildEngage();
            case GroupActions::FIGHT:
                return BuildFight();
            case GroupActions::TRAVEL:
                return BuildTravel();
        }
    }

    const GroupAction * GroupActionFactory::BuildTakeShortRest()
    {
        static GroupAction action = {
            GroupActions::TAKE_SHORT_REST, 
            18 * GroupAction::ACTION_PROGRESS_RATE,
            false, 
            &GroupActionValidator::CanTakeShortRest, 
            &GroupActionPerformer::TakeShortRest};
        return &action;
    }

    const GroupAction * GroupActionFactory::BuildTakeLongRest()
    {
        static GroupAction action = {
            GroupActions::TAKE_LONG_REST, 
            48 * GroupAction::ACTION_PROGRESS_RATE,
            false,
            &GroupActionValidator::CanTakeLongRest, 
            &GroupActionPerformer::TakeLongRest};
        return &action;
    }

    const GroupAction * GroupActionFactory::BuildSearch()
    {
        static GroupAction action = {
            GroupActions::SEARCH, 
            18 * GroupAction::ACTION_PROGRESS_RATE, 
            false,
            &GroupActionValidator::CanSearch, 
            &GroupActionPerformer::Search
            };
        return &action;
    }

    const GroupAction * GroupActionFactory::BuildEngage()
    {
        static GroupAction action = {
            GroupActions::ENGAGE, 
            0 * GroupAction::ACTION_PROGRESS_RATE,
            false,
            &GroupActionValidator::CanEngage, 
            &GroupActionPerformer::Engage
            };
        return &action;
    }

    const GroupAction * GroupActionFactory::BuildFight()
    {
        static GroupAction action = {
            GroupActions::FIGHT, 
            0 * GroupAction::ACTION_PROGRESS_RATE,
            false,
            &GroupActionValidator::CanFight, 
            &GroupActionPerformer::Fight
            };
        return &action;
    }

    const GroupAction * GroupActionFactory::BuildTravel()
    {
        static GroupAction action = {
            GroupActions::TRAVEL, 
            36 * GroupAction::ACTION_PROGRESS_RATE,
            true, 
            &GroupActionValidator::CanTravel, 
            &GroupActionPerformer::Travel, 
            &GroupActionPerformer::InitiateTravel, 
            &GroupActionPerformer::GetTravelDuration
            };
        return &action;
    }

    GroupActionResult GroupActionPerformer::InitiateTravel(Group &group, const GroupActionData &actionData)
    {
        if(group.travelActionData.Destination != actionData.TravelDestination)
        {
            group.travelActionData.Progress = 0;

            group.travelActionData.Destination = actionData.TravelDestination;
            group.travelActionData.Source = group.GetTile();
        }

        /*auto difficultyClass = 15;

        difficultyClass += group.tile->Biome->TravelPenalty;
        difficultyClass += group.travelActionData.Destination->Biome->TravelPenalty;

        int modifier = -100;
        for(auto &character : group.characters)
        {
            if(character.GetSkillProficiency(SkillTypes::SURVIVAL) > modifier)
                modifier = character.GetSkillProficiency(SkillTypes::SURVIVAL);
        }

        group.actionSuccess = utility::RollD20Dice(difficultyClass, modifier);

        return {group.actionSuccess.GetRollValue(), modifier, difficultyClass, SkillTypes::SURVIVAL};*/
    }

    int GroupActionPerformer::GetTravelDuration(const Group &group)
    {
        auto durationModifier = 0;

        durationModifier += group.tile->GetTravelPenalty();
        durationModifier += group.travelActionData.Destination->GetTravelPenalty();

        return group.action->BaseDuration + durationModifier * 6 * GroupAction::ACTION_PROGRESS_RATE;
    }

    GroupActionResult GroupActionPerformer::TakeShortRest(Group& group)
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

    GroupActionResult GroupActionPerformer::TakeLongRest(Group& group)
    {
        if(group.actionProgress != group.action->BaseDuration)
            return;

        auto &characters = group.GetCharacters();
        for(auto &character : group.characters)
        {
            character.TakeLongRest();
        }

        group.timeSinceLongRest = 0;

        group.CancelAction();
    }

    GroupActionResult GroupActionPerformer::Search(Group& group)
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
        if(perceptionCheck <= 30)
            return;

        if(group.actionProgress < group.action->BaseDuration)
            return;

        group.CancelAction();

        WorldScene::Get()->StartBattle(&group, other);
    }

    GroupActionResult GroupActionPerformer::Fight(Group& group)
    {
        
    }

    GroupActionResult GroupActionPerformer::Engage(Group& group)
    {
        
    }

    #define SURVIVAL_DC_WHEN_NOT_LOST 5

    #define SURVIVAL_DC_WHEN_LOST 10

    GroupActionResult GroupActionPerformer::Travel(Group& group)
    {
        if(group.travelActionData.IsLost == false)
        {
            group.travelActionData.Progress += group.GetProgressRate();
        }

        group.travelActionData.ProgressSinceCheck += group.GetProgressRate();

        static const int CHECK_INTERVAL = 6 * GroupAction::ACTION_PROGRESS_RATE;
        if(group.travelActionData.ProgressSinceCheck > CHECK_INTERVAL)
        {
            group.travelActionData.ProgressSinceCheck -= CHECK_INTERVAL;

            auto difficultyClass = group.travelActionData.IsLost ? SURVIVAL_DC_WHEN_LOST : SURVIVAL_DC_WHEN_NOT_LOST;

            difficultyClass += group.travelActionData.Source->GetTravelPenalty();
            difficultyClass += group.travelActionData.Destination->GetTravelPenalty();

            int modifier = -100;
            for(auto &character : group.characters)
            {
                if(character.GetSkillProficiency(SkillTypes::SURVIVAL) > modifier)
                    modifier = character.GetSkillProficiency(SkillTypes::SURVIVAL);
            }

            auto success = utility::RollD20Dice(difficultyClass, modifier);
            if(success.IsAnyFailure() == true)
            {
                group.travelActionData.IsLost = true;
            }
            else
            {
                group.travelActionData.IsLost = false;
            }

            return {success, SkillTypes::SURVIVAL};
        }

        if(group.travelActionData.Progress < group.action->GetDuration(group))
            return;

        group.SetTile(group.travelActionData.Destination);
        group.travelActionData.Destination = nullptr;
        group.travelActionData.Source = nullptr;

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

    bool GroupActionValidator::CanEngage(Group &group, const GroupActionData &)
    {
        return true;
    }

    bool GroupActionValidator::CanTravel(Group &group, const GroupActionData &data)
    {
        if((group.travelActionData.Destination == nullptr && data.TravelDestination == nullptr) ||
        (group.travelActionData.Destination != nullptr && data.TravelDestination != group.travelActionData.Destination))
            return false;

        if(data.TravelDestination->Type == WorldTiles::SEA || data.TravelDestination->GetGroup() != nullptr)
            return false;

        if(data.TravelDestination->GetDistanceTo(*group.tile) != 1)
            return false;
            
        return true;
    }
}