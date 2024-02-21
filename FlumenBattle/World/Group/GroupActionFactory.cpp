#include "FlumenBattle/World/Group/GroupActionFactory.h"
#include "FlumenBattle/World/Group/GroupAction.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Group/GroupBatchMap.h"
#include "FlumenBattle/World/Group/Encounter.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/WorldBiome.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/Utility/Utility.h"
#include "FlumenBattle/Config.h"

static const auto NOURISHED_DURATION = 8 * 6;

static const auto BASE_FORAGE_DC = 15;

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
            case GroupActions::DISENGAGE:
                return BuildDisengage();
            case GroupActions::TRAVEL:
                return BuildTravel();
            case GroupActions::PERSUADE:
                return BuildPersuade();
            case GroupActions::FORAGE:
                return BuildForage();
        }
    }

    const GroupAction * GroupActionFactory::BuildTakeShortRest()
    {
        static GroupAction action = {
            GroupActions::TAKE_SHORT_REST, 
            3 * WorldTime::HOUR_SIZE * GroupAction::BASE_PROGRESS_RATE,
            false, 
            &GroupActionValidator::CanTakeShortRest, 
            &GroupActionPerformer::TakeShortRest};
        return &action;
    }

    const GroupAction * GroupActionFactory::BuildTakeLongRest()
    {
        static GroupAction action = {
            GroupActions::TAKE_LONG_REST, 
            8 * WorldTime::HOUR_SIZE * GroupAction::BASE_PROGRESS_RATE,
            false,
            &GroupActionValidator::CanTakeLongRest, 
            &GroupActionPerformer::TakeLongRest};
        return &action;
    }

    const GroupAction * GroupActionFactory::BuildSearch()
    {
        static GroupAction action = {
            GroupActions::SEARCH, 
            30 * WorldTime::HOUR_SIZE * GroupAction::BASE_PROGRESS_RATE, 
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
            0 * GroupAction::BASE_PROGRESS_RATE,
            false,
            &GroupActionValidator::CanEngage, 
            &GroupActionPerformer::Engage,
            &GroupActionPerformer::InitiateEngage
            };
        return &action;
    }

    const GroupAction * GroupActionFactory::BuildFight()
    {
        static GroupAction action = {
            GroupActions::FIGHT, 
            0 * GroupAction::BASE_PROGRESS_RATE,
            false,
            &GroupActionValidator::CanFight, 
            &GroupActionPerformer::Fight
            };
        return &action;
    }

    const GroupAction * GroupActionFactory::BuildDisengage()
    {
        static GroupAction action = {
            GroupActions::DISENGAGE, 
            0 * GroupAction::BASE_PROGRESS_RATE,
            false,
            &GroupActionValidator::CanDisengage, 
            &GroupActionPerformer::Disengage
            };
        return &action;
    }

    const GroupAction * GroupActionFactory::BuildTravel()
    {
        static GroupAction action = {
            GroupActions::TRAVEL, 
            6 * WorldTime::HOUR_SIZE * GroupAction::BASE_PROGRESS_RATE,
            true, 
            &GroupActionValidator::CanTravel, 
            &GroupActionPerformer::Travel, 
            &GroupActionPerformer::InitiateTravel, 
            &GroupActionPerformer::GetTravelDuration
            };
        return &action;
    }

    const GroupAction * GroupActionFactory::BuildPersuade()
    {
        static GroupAction action = {
            GroupActions::PERSUADE, 
            0 * GroupAction::BASE_PROGRESS_RATE,
            false, 
            &GroupActionValidator::CanPersuade, 
            &GroupActionPerformer::Persuade 
            };
        return &action;
    }

    const GroupAction * GroupActionFactory::BuildForage()
    {
        static GroupAction action = {
            GroupActions::FORAGE, 
            3 * WorldTime::HOUR_SIZE * GroupAction::BASE_PROGRESS_RATE,
            false, 
            &GroupActionValidator::CanForage, 
            &GroupActionPerformer::Forage 
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

            group.travelActionData.Duration = group.action->GetDuration(group);
        }
    }

    GroupActionResult GroupActionPerformer::InitiateEngage(Group &group, const GroupActionData &data)
    {
        if(data.IsEngaged == false)
        {
            group.attitude = utility::GetRandom(1, 20) > 15 ? Attitudes::HOSTILE : Attitudes::INDIFFERENT;

            group.hasAttemptedPersuasion = false;
        }
    }

    int GroupActionPerformer::GetTravelDuration(const Group &group)
    {
        auto durationModifier = 0;

        durationModifier += group.travelActionData.Source->GetTravelPenalty().Value;
        durationModifier += group.travelActionData.Destination->GetTravelPenalty().Value;

        return group.action->BaseDuration + durationModifier * WorldTime::HOUR_SIZE * GroupAction::BASE_PROGRESS_RATE;
    }

    GroupActionResult GroupActionPerformer::TakeShortRest(Group& group)
    {
        if(group.actionProgress != group.action->BaseDuration)
            return {};
        
        auto &characters = group.GetCharacters();
        for(auto &character : characters)
        {
            character.TakeShortRest();
        }

        group.CancelAction();

        return {GroupActions::TAKE_SHORT_REST};
    }

    GroupActionResult GroupActionPerformer::TakeLongRest(Group& group)
    {
        if(group.actionProgress != group.action->BaseDuration)
            return {};

        auto &characters = group.GetCharacters();
        for(auto &character : group.characters)
        {
            character.TakeLongRest();
        }

        group.timeSinceLongRest = 0;

        auto foodNeeded = group.GetFoodConsumption();
        auto foodAvailable = group.GetItemAmount(character::ItemTypes::FOOD);
        
        if(foodAvailable >= foodNeeded)
        {
            group.RemoveItemAmount(character::ItemTypes::FOOD, foodNeeded);
            for(auto &character : group.characters)
            {
                character.AddCondition({character::Conditions::NOURISHED, NOURISHED_DURATION});
            }
        }

        auto isWinter = group.GetTile()->IsWinter();
        if(isWinter == true && group.GetCurrentSettlement() == nullptr)
        {
            auto survivalBonus = group.GetMostSkilledMember(character::SkillTypes::SURVIVAL).Bonus;

            static const auto WINTER_SURVIVAL_DC = engine::ConfigManager::Get()->GetValue(game::ConfigValues::WINTER_SURVIVAL_DC).Integer;
            auto survivalCheck = utility::RollD20Dice(WINTER_SURVIVAL_DC, survivalBonus);
            if(survivalCheck.IsAnyFailure() == true)
            {
                for(auto &character : group.characters)
                {
                    auto fortitudeBonus = character.GetFortitudeSaveBonus();

                    static const auto WINTER_FROSTBITE_DC = engine::ConfigManager::Get()->GetValue(game::ConfigValues::WINTER_FROSTBITE_DC).Integer;
                    auto frostBiteCheck = utility::RollD20Dice(WINTER_FROSTBITE_DC, fortitudeBonus);
                    if(frostBiteCheck.IsCriticalSuccess() == false)
                    {
                        auto damage = utility::RollDice({utility::RollDies::D4, 1, 0});
                        if(frostBiteCheck.IsNormalSuccess() == true)
                        {
                            character.SufferDamage(damage / 2);
                        }
                        else if(frostBiteCheck.IsRegularFailure() == true)
                        {
                            character.SufferDamage(damage);
                        }
                        else
                        {
                            character.SufferDamage(damage * 2);
                        }
                    }
                }
            }

            group.CancelAction();

            return {GroupActions::TAKE_LONG_REST, survivalCheck, character::SkillTypes::SURVIVAL};
        }
        else
        {
            group.CancelAction();

            return {};
        }
    }

    GroupActionResult GroupActionPerformer::Search(Group& group)
    {
        if(group.actionProgress == group.action->BaseDuration)
        {
            group.CancelAction();

            return {};
        }

        static const auto MAXIMUM_GROUP_SPOTTING_DISTANCE = engine::ConfigManager::Get()->GetValue(game::ConfigValues::MAXIMUM_GROUP_SPOTTING_DISTANCE).Integer;

        auto nearbyGroups = WorldScene::Get()->GetNearbyGroups(group.GetTile(), MAXIMUM_GROUP_SPOTTING_DISTANCE);
        
        if(nearbyGroups.Groups.GetSize() == 1)
            return {};

        auto spottedGroup = [&]
        {
            while(true)
            {
                auto other = *nearbyGroups.Groups.GetRandom();

                if(other != &group)
                    return other;
            }
        } ();

        auto perceptionBonus = group.GetMostSkilledMember(character::SkillTypes::PERCEPTION).Bonus;

        const auto distance = group.GetDistanceTo(spottedGroup);
        if(distance == 0)
        {
            perceptionBonus += 1;
        }
        else if(distance == MAXIMUM_GROUP_SPOTTING_DISTANCE)
        {
            perceptionBonus -= 2;
        }

        static const auto TERRAIN_PERCEPTION_PENALTY = engine::ConfigManager::Get()->GetValue(game::ConfigValues::TERRAIN_PERCEPTION_PENALTY).Integer;

        auto stealthBonus = spottedGroup->GetLeastSkilledMember(character::SkillTypes::STEALTH).Bonus;
        if(spottedGroup->GetTile()->HasBiome(WorldBiomes::WOODS) || spottedGroup->GetTile()->HasRelief(WorldReliefs::MOUNTAINS))
        {
            stealthBonus += TERRAIN_PERCEPTION_PENALTY;
        }

        static const auto NIGHT_PERCEPTION_PENALTY = engine::ConfigManager::Get()->GetValue(game::ConfigValues::NIGHT_PERCEPTION_PENALTY).Integer;

        auto isNight = WorldScene::Get()->IsNightTime();
        if(isNight == true)
        {
            stealthBonus += NIGHT_PERCEPTION_PENALTY;
        }

        auto modifier = perceptionBonus - stealthBonus;

        static const auto GROUP_SEARCH_BASE_DC = engine::ConfigManager::Get()->GetValue(game::ConfigValues::GROUP_SEARCH_BASE_DC).Integer;

        auto perceptionCheck = utility::RollD20Dice(GROUP_SEARCH_BASE_DC, modifier);
        if(perceptionCheck.IsAnySuccess() == true)
        {
            return {GroupActions::SEARCH, perceptionCheck, character::SkillTypes::PERCEPTION, spottedGroup};
        }

        return {};
    }

    GroupActionResult GroupActionPerformer::Fight(Group& group)
    {
        
    }

    GroupActionResult GroupActionPerformer::Engage(Group& group)
    {
        
    }

    GroupActionResult GroupActionPerformer::Disengage(Group& group)
    {
        if(group.encounter->IsWinner(&group) == true)
        {
            group.money += group.GetOther()->money;
        }
        else if(group.encounter->IsWinner(group.GetOther()) == true)
        {
            group.GetOther()->money += group.money;
        }

        group.CancelAction();

        group.GetOther()->CancelAction();

        group.GetOther()->encounter = nullptr;

        group.encounter = nullptr;

        return {GroupActions::DISENGAGE};
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

        static const int CHECK_INTERVAL = 6 * GroupAction::BASE_PROGRESS_RATE;
        if(group.travelActionData.ProgressSinceCheck > CHECK_INTERVAL)
        {
            group.travelActionData.ProgressSinceCheck -= CHECK_INTERVAL;

            auto difficultyClass = group.travelActionData.IsLost ? SURVIVAL_DC_WHEN_LOST : SURVIVAL_DC_WHEN_NOT_LOST;

            difficultyClass += group.travelActionData.Source->GetTravelPenalty().Value;
            difficultyClass += group.travelActionData.Destination->GetTravelPenalty().Value;

            auto survivalBonus = group.GetMostSkilledMember(character::SkillTypes::SURVIVAL).Bonus;

            auto roadBonus = group.travelActionData.Source->IsLinkedTo(group.travelActionData.Destination) ? 4 : 0;

            auto success = utility::RollD20Dice(difficultyClass, survivalBonus + roadBonus);
            if(success.IsAnyFailure() == true)
            {
                group.travelActionData.IsLost = true;
            }
            else
            {
                group.travelActionData.IsLost = false;
            }

            return {GroupActions::TRAVEL, success, character::SkillTypes::SURVIVAL};
        }

        if(group.travelActionData.Progress < group.action->GetDuration(group) / 2)
            return {};

        group.SetTile(group.travelActionData.Destination);

        if(group.travelActionData.Progress < group.action->GetDuration(group))
            return {};

        //group.SetTile(group.travelActionData.Destination);
        group.travelActionData.Destination = nullptr;
        group.travelActionData.Source = nullptr;
        group.travelActionData.PlannedDestinationCount--;
        if(group.travelActionData.PlannedDestinationCount == 0)
        {
            group.travelActionData.IsOnRoute = false;
        }
        else
        {
            for(int i = 0; i < group.travelActionData.PlannedDestinationCount; ++i)
            {
                group.travelActionData.Route[i] = group.travelActionData.Route[i + 1];
            }
        }

        group.CancelAction();

        return {};
    }

    GroupActionResult GroupActionPerformer::Persuade(Group& group)
    {
        auto difficultyClass = 10 + group.GetOther()->GetLeader()->GetWillSaveBonus();

        auto modifier = group.GetLeader()->GetSkillProficiency(character::SkillTypes::PERSUASION);

        auto success = utility::RollD20Dice(difficultyClass, modifier);

        if(success.IsAnySuccess() == true)
        {
            group.GetOther()->attitude = Attitudes::INDIFFERENT;
        }

        group.hasAttemptedPersuasion = true;

        group.SelectAction(GroupActions::ENGAGE, {true});

        return {GroupActions::PERSUADE, success, character::SkillTypes::PERSUASION};
    }

    GroupActionResult GroupActionPerformer::Forage(Group& group)
    {
        if(group.actionProgress != group.action->BaseDuration)
            return {};

        auto survivalBonus = group.GetMostSkilledMember(character::SkillTypes::SURVIVAL).Bonus;

        auto difficultyClass = BASE_FORAGE_DC;

        if(group.GetTile()->HasRelief(WorldReliefs::MOUNTAINS) == true)
        {
            difficultyClass += 4;
        }
        else if(group.GetTile()->HasBiome(WorldBiomes::DESERT) == true)
        {
            difficultyClass += 4;
        }
        else if(group.GetTile()->HasBiome(WorldBiomes::STEPPE) == true && group.GetTile()->IsScrubland == true)
        {
            difficultyClass += 2;
        }

        if(group.GetTile()->IsWinter() == true)
        {
            difficultyClass += 4;
        }
        
        auto skillCheck = utility::RollD20Dice(difficultyClass, survivalBonus);

        auto foragedFood = [&skillCheck]
        {
            auto amount = utility::RollDice({utility::RollDies::D4, 2});
            if(skillCheck.IsCriticalSuccess() == true)
            {
                return amount * 2;
            }
            else if(skillCheck.IsAnySuccess() == true)
            {
                return amount;
            }
            else if(skillCheck.IsRegularFailure() == true)
            {
                return amount / 2;
            }
            else
            {
                return 0;
            }
        } ();

        group.AddItem(character::ItemTypes::FOOD, foragedFood);

        group.CancelAction();

        return {GroupActions::FORAGE, skillCheck, character::SkillTypes::SURVIVAL};
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
        return group.IsDoing(GroupActions::ENGAGE) && group.GetEncounter()->HasBattleEnded() == false;
    }

    bool GroupActionValidator::CanEngage(Group &group, const GroupActionData &)
    {
        return true;
    }

    bool GroupActionValidator::CanDisengage(Group &group, const GroupActionData &)
    {
        if(group.GetEncounter()->HasBattleEnded() == true)
        {
            return true;
        }
        else
        {
            return group.IsDoing(GroupActions::ENGAGE) && group.GetOther()->attitude != Attitudes::HOSTILE;
        }
    }

    bool GroupActionValidator::CanTravel(Group &group, const GroupActionData &data)
    {
        //if(group.travelActionData.Destination != nullptr && group.GetTravelProgress() > 0.5f)
            //return false;

        /*if((group.travelActionData.Destination == nullptr && data.TravelDestination == nullptr) ||
        (group.travelActionData.Destination != nullptr && data.TravelDestination != group.travelActionData.Destination))
            return false;*/

        if(data.TravelDestination->Type == WorldTiles::SEA)
            return false;

        if(data.TravelDestination->GetDistanceTo(*group.tile) != 1)
            return false;
            
        return true;
    }

    bool GroupActionValidator::CanPersuade(Group &group, const GroupActionData &data)
    {
        return group.IsDoing(GroupActions::ENGAGE) && 
        group.GetOther()->attitude == Attitudes::HOSTILE && 
        group.GetEncounter()->HasBattleEnded() == false &&
        group.hasAttemptedPersuasion == false;
    }

    bool GroupActionValidator::CanForage(Group &group, const GroupActionData &)
    {
        return true;
    }
}