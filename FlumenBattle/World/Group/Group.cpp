#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Group/GroupType.h"
#include "FlumenBattle/World/Group/Encounter.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/WorldTime.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/World/Character/CharacterFactory.h"
#include "FlumenBattle/RaceFactory.h"
#include "FlumenBattle/World/Character/ClassFactory.h"
#include "FlumenBattle/World/Group/GroupAction.h"
#include "FlumenBattle/World/Group/GroupActionFactory.h"
#include "FlumenBattle/World/Group/GroupMind.h"
#include "FlumenBattle/World/Group/GroupAllocator.h"
#include "FlumenBattle/World/Group/Types.h"
#include "FlumenBattle/World/Group/GroupBatch.h"
#include "FlumenBattle/World/Group/GroupBatchMap.h"
#include "FlumenBattle/World/Polity/Polity.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/Config.h"

using namespace world::character;

#define FATIGUE_SAVING_THROW_DC 12

#define MULE_CARRY_CAPACITY 25

#define MULE_FOOD_CONSUMPTION 3

namespace world::group
{
    Group::Group() {}

    void *Group::operator new(size_t size)
    {
        return GroupAllocator::Get()->Allocate();
    }

    void Group::Initialize(const GroupType *groupType, Integer size, Color color, RaceTypes raceType)
    {
        this->type = groupType;
        this->controller = groupType->Controller;

        this->color = color;

        this->isAlive = true;
        this->hasAchievedObjective = false;

        if(this->type->Class == GroupClasses::ADVENTURER)
        {
            this->money = utility::GetRandom(100, 150);
        }
        else if(this->type->Class == GroupClasses::MERCHANT)
        {
            this->money = utility::GetRandom(200, 300);
        }
        else if(this->type->Class == GroupClasses::PLAYER)
        {
            this->money = utility::GetRandom(100, 150);
        }
        else if(this->type->Class == GroupClasses::GARRISON || this->type->Class == GroupClasses::PATROL)
        {
            this->money = utility::GetRandom(50, 100);
        }
        else if(this->type->Class == GroupClasses::BANDIT)
        {
            this->money = utility::GetRandom(100, 150);
        }

        actionProgress = 0;

        timeSinceLongRest = 0;

        action = nullptr;
        encounter = nullptr;
        tile = nullptr;
        home = nullptr;
        domain = nullptr;

        leader = nullptr;
        auto charismaModifier = -100;
        for(auto &character : characters)
        {
            auto modifier = character.GetAbility(AbilityTypes::CHARISMA).Modifier;
            if(modifier > charismaModifier)
            {
                charismaModifier = modifier;
                leader = &character;
            }
        }

        items.Add(character::ItemTypes::SWORD);
        items.Add(character::ItemTypes::ARMOR);
        items.Add(character::ItemTypes::ARMOR);
        items.Add(character::ItemTypes::HELMET);
        items.Add(character::ItemTypes::HELMET);
        items.Add(character::ItemTypes::AXE);
        items.Add(character::ItemTypes::SHIELD);
        items.Add(character::ItemTypes::ARMOR);
        items.Add(character::ItemTypes::SPEAR);
        auto firstBow = items.Add(character::ItemTypes::BOW);
        auto secondBow = items.Add(character::ItemTypes::BOW);

        auto index = 0;
        for(auto &character : characters)
        {
            if(character.GetClass()->Class == character::CharacterClasses::RANGER)
            {
                character.EquipItem(firstBow, character::ItemPositions::MAIN_HAND);
                index++;

                if(index == 1)
                {
                    break;
                }
            }
        }
    }

    bool Group::IsAlive()
    {
        if(isAlive == false)
            return false;

        for(auto &character : characters)
        {
            if(character.IsAlive())
            {
                return true;
            }
        }

        return false;
    }

    void Group::Update()
    {
        for(auto &character : characters)
        {
            character.Update();
        }
    }

    void Group::CheckFatigue()
    {
        if(this->IsDoing(GroupActions::TAKE_LONG_REST) == true)
            return;

        timeSinceLongRest += 1;

        static const auto FATIGUE_ONSET_SINCE_REST = engine::ConfigManager::Get()->GetValue(game::ConfigValues::FATIGUE_ONSET_SINCE_REST).Integer;

        static const auto TIME_UNTIL_FATIGUE_CHECK = FATIGUE_ONSET_SINCE_REST * WorldTime::HOUR_SIZE;

        if(timeSinceLongRest < TIME_UNTIL_FATIGUE_CHECK || (timeSinceLongRest - TIME_UNTIL_FATIGUE_CHECK) % WorldTime::HOUR_SIZE != 0)
        {
            return;
        }

        for(auto &character : characters)
        {
            auto bonus = character.GetFortitudeSaveBonus();
            auto savingThrow = utility::RollD20Dice(FATIGUE_SAVING_THROW_DC, bonus);
            if(savingThrow.IsAnyFailure())
            {
                character.AddCondition({Conditions::FATIGUE});
            }
        }
    }

    void Group::DetermineAction()
    {
        controller->DetermineAction(*this);
    }

    bool Group::ValidateAction(GroupActions actionType, const GroupActionData &actionData)
    {
        auto possibleAction = GroupActionFactory::Get()->BuildAction(actionType);

        return possibleAction->CanPerform(*this, actionData);
    }   

    GroupActionResult Group::SelectAction(GroupActions actionType, const GroupActionData &actionData)
    {
        if(action && action->Type == actionType)
            return {};

        action = GroupActionFactory::Get()->BuildAction(actionType);

        actionProgress = 0;

        travelActionData.Intensity = ActionIntensities::NORMAL;

        auto result = action->Initiate(*this, actionData);
        result.Content.isAlreadyEngaged = actionData.IsEngaged;

        controller->RegisterActionInitiation(*this, result);

        return result;
    }

    void Group::PerformAction()
    {
        CheckFatigue();

        if(action)
        {
            actionProgress += GetProgressRate();

            auto result = action->Perform(*this);

            controller->RegisterActionPerformance(*this, result);
        }
    }

    void Group::CancelAction()
    {
        action = nullptr;

        actionProgress = 0;
    }

    void Group::IntensifyAction()
    {
        if(action == nullptr)
            return;
            
        if(action->HasVaryingIntensity == false)
            return;

        switch(travelActionData.Intensity)
        {
            case ActionIntensities::LEISURELY:
                travelActionData.Intensity = ActionIntensities::NORMAL;
                break;
            case ActionIntensities::NORMAL:
                travelActionData.Intensity = ActionIntensities::INTENSE;
                break;
        }
    }

    void Group::SlackenAction()
    {
        if(action == nullptr)
            return;

        if(action->HasVaryingIntensity == false)
            return;
            
        switch(travelActionData.Intensity)
        {
            case ActionIntensities::NORMAL:
                travelActionData.Intensity = ActionIntensities::LEISURELY;
                break;
            case ActionIntensities::INTENSE:
                travelActionData.Intensity = ActionIntensities::NORMAL;
                break;
        }
    }

    void Group::EngageGroup(Encounter *_encounter)
    {
        encounter = _encounter;

        SelectAction(GroupActions::ENGAGE, {false});
    }

    void Group::ExitBattle()
    {
        encounter = nullptr;

        CancelAction();
    }

    void Group::SetTile(WorldTile *newTile)
    {
        if(this->tile == newTile)
            return;

        auto oldBatch = GroupBatchMap::Get()->GetBatch(this->tile);
        if(oldBatch != nullptr)
        {
            oldBatch->Remove(this);
        }

        auto newBatch = GroupBatchMap::Get()->GetBatch(newTile);
        newBatch->Add(this);

        this->tile = newTile;
    }

    void Group::AddItem(character::ItemTypes type, int amount)
    {
        items.Add(type, amount);
    }

    void Group::RemoveItem(character::Item *item)
    {
        items.Remove(item);
    }

    void Group::RemoveItemAmount(character::ItemTypes type, int amount)
    {
        items.RemoveAmount(type, amount);
    }

    float Group::GetActionProgress() const
    {
        if(action->HasVaryingIntensity == false)
        {
            return (float)actionProgress / (float)action->GetDuration(*this);   
        }
        else
        {
            return (float)travelActionData.Progress / (float)travelActionData.Duration;   
        }
    }

    float Group::GetTravelProgress() const
    {
        return (float)travelActionData.Progress / (float)travelActionData.Duration;//->GetDuration(*this);   
    }

    int Group::GetRemainingActionDuration() const
    {
        if(action->HasVaryingIntensity == false)
        {
            return action->GetDuration(*this) - actionProgress;
        }
        else
        {
            return action->GetDuration(*this) - travelActionData.Progress;
        }
    }

    int Group::GetProgressRate() const
    {
        if(action->HasVaryingIntensity == false)
        {
            return GroupAction::BASE_PROGRESS_RATE;
        }

        auto encumbrancePenalty = IsEncumbered() == true ? GroupAction::BASE_ENCUMBRANCE_PENALTY : 0;

        switch(travelActionData.Intensity)
        {
            case ActionIntensities::LEISURELY:
                return GroupAction::BASE_PROGRESS_RATE / 2;
            case ActionIntensities::NORMAL:
                return GroupAction::BASE_PROGRESS_RATE - encumbrancePenalty;
            case ActionIntensities::INTENSE:
                return GroupAction::BASE_PROGRESS_RATE + (GroupAction::BASE_PROGRESS_RATE / 2) - encumbrancePenalty;
        }
    }

    bool Group::IsDoing(GroupActions actionType) const 
    {
        return action != nullptr ? action->Type == actionType : false;
    }

    bool Group::IsDoingSomething() const 
    {
        return action != nullptr;
    }

    bool Group::IsInEncounter() const
    {
        return IsDoing(GroupActions::ENGAGE) == true || IsDoing(GroupActions::FIGHT) == true;
    }

    character::Character *Group::GetCharacter(int index) 
    {
        return characters.Get(index);
    }

    int Group::GetLivingCount() const
    {
        auto count = 0;
        for(auto &character : characters)
        {
            if(character.IsAlive() == true)
                count++;
        }

        return count;
    }

    SkillData Group::GetMostSkilledMember(SkillTypes skill) const
    {
        int bonus = INT_MIN;
        character::Character *mostSkilledMember = nullptr;
        for(auto &character : characters)
        {
            if(character.GetSkillProficiency(skill) > bonus)
            {
                bonus = character.GetSkillProficiency(skill);
                mostSkilledMember = &character;
            }
        }

        return {mostSkilledMember, bonus};
    }

    SkillData Group::GetLeastSkilledMember(SkillTypes skill) const
    {
        int bonus = INT_MAX;
        character::Character *mostSkilledMember = nullptr;
        for(auto &character : characters)
        {
            if(character.GetSkillProficiency(skill) < bonus)
            {
                bonus = character.GetSkillProficiency(skill);
                mostSkilledMember = &character;
            }
        }

        return {mostSkilledMember, bonus};
    }

    Group *Group::GetOther()
    {
        if(encounter == nullptr)
        {
            return nullptr;
        }

        if(encounter->GetAttacker() == this)
        {
            return encounter->GetDefender();
        }

        return encounter->GetAttacker();
    }

    world::character::Item *Group::GetItem(int index)
    {
        return items.GetItem(index);
    }

    int Group::GetItemAmount(ItemTypes type)
    {
        return items.GetAmount(type);
    }

    const container::Pool <character::Item> &Group::GetItems() const
    {
        return items.GetItems();
    }

    int Group::GetFoodConsumption() const
    {
        return characters.GetSize() + muleCount * MULE_FOOD_CONSUMPTION;
    }

    GroupClasses Group::GetClass() const
    {
        return type->Class;
    }

    WorldTile *Group::GetFinalDestination() const
    {
        return travelActionData.IsOnRoute == true ? travelActionData.Route[travelActionData.PlannedDestinationCount - 1] : nullptr;
    }

    settlement::Settlement *Group::GetCurrentSettlement()
    {
        if(IsDoing(GroupActions::TRAVEL))
            return nullptr;

        return tile->GetSettlement();
    }

    int Group::GetCarryCapacity() const
    {
        int strengthScoreSum = 0;
        for(auto &character : characters)
        {
            if(character.IsAlive() == false)
                continue;

            strengthScoreSum += character.GetAbility(character::AbilityTypes::STRENGTH).Score;
        }

        auto muleCapacity = muleCount * MULE_CARRY_CAPACITY;

        return strengthScoreSum + muleCapacity;
    }

    int Group::GetCarriedWeight() const
    {
        return items.GetTotalWeight();
    }

    bool Group::IsEncumbered() const
    {
        return GetCarriedWeight() > GetCarryCapacity();
    }

    Position2 Group::GetVisualPosition() const
    {
        if(GetDestination() != nullptr)
        {   
            auto progress = GetTravelProgress();

            auto startPosition = GetDestination()->Position;
            auto endPosition = GetTravelStartPoint()->Position;
            return endPosition * (1.0f - progress) + startPosition * progress;
        }
        else
        {
            return tile->Position;
        }
    }

    int Group::GetDistanceTo(const Group *group) const
    {
        return tile->GetDistanceTo(*group->GetTile());
    }

    bool Group::DoesRulePolity() const
    {
        return domain != nullptr;
    }

    void Group::SetDomain(polity::Polity *polity)
    {
        domain = polity;
    }

    int Group::GetDomainSettlementCount() const
    {
        return domain->GetSettlements().GetSize();
    }

    bool Group::HasSameAllegiance(const Group &other) const
    {
        return GetHome()->GetPolity() == other.GetHome()->GetPolity();
    }
}
