#include <type_traits>

#include "GroupCore.h"
#include "GroupExtraData.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/World/Character/ClassFactory.h"
#include "FlumenBattle/World/Character/CharacterClass.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/Group/GroupAction.h"
#include "FlumenBattle/World/Group/GroupBatch.h"
#include "FlumenBattle/World/Group/GroupType.h"
#include "FlumenBattle/World/Group/Encounter.h"
#include "FlumenBattle/World/Group/GroupMind.h"
#include "FlumenBattle/World/Group/GroupActionFactory.h"
#include "FlumenBattle/World/Group/MachineMind.h"
#include "FlumenBattle/World/Group/GroupAllocator.h"
#include "FlumenBattle/World/Group/CharacterEssence.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Polity/Polity.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Character/NameGenerator.h"
#include "FlumenBattle/Config.h"

using namespace world::group;

#define FATIGUE_SAVING_THROW_DC 12

#define MULE_CARRY_CAPACITY 25

#define MULE_FOOD_CONSUMPTION 3

CharacterEssence *CharacterHandler::AddCharacter()
{
    auto characterEssence = characters->Add();

    characterCount++;

    return characterEssence;
}

void CharacterHandler::DamageCharacter(CharacterEssence &character, int damage)
{
    bool hasDied = character.SufferDamage(damage);
    if(hasDied == true)
    {
        characterCount--;
    }
}

void GroupCore::Initialize(const GroupType *groupType, Integer size, RaceTypes raceType)
{
    this->type = groupType;

    this->isAlive = true;
    this->hasAchievedObjective = false;

    this->hasMission = false;
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

    if(extraData != nullptr)
    {
        extraData->Initialize(this);
    }
}

bool GroupCore::IsAlive()
{
    if(extraData != nullptr)
    {
        return extraData->IsAlive();
    }

    if(isAlive == false)
        return false;

    for(auto &character : *characterHandler.characters)
    {
        if(character.isAlive == true)
        {
            return true;
        }
    }

    return false;
}

Position2 GroupCore::GetVisualPosition() const
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

bool GroupCore::IsDoing(GroupActions actionType) const 
{
    return action != nullptr ? action->Type == actionType : false;
}

bool GroupCore::IsDoingSomething() const 
{
    return action != nullptr;
}

bool GroupCore::IsInEncounter() const
{
    return IsDoing(GroupActions::ENGAGE) == true || IsDoing(GroupActions::FIGHT) == true;
}

Pool <world::character::Character> &GroupCore::GetCharacters() 
{
    if(extraData != nullptr)
        return extraData->GetCharacters();

    std::cout << "Calling GroupCore GetCharacters when ExtraData is missing." << std::endl;
    std::abort();
}

int GroupCore::GetLivingCount() const
{
    if(extraData != nullptr)
    {
        return extraData->GetLivingCount();
    }
    
    auto count = 0;
    for(auto &character : *characterHandler.characters)
    {
        if(character.isAlive == true)
            count++;
    }

    return count;
}

world::character::Character *GroupCore::GetCharacter(int index)
{
    return extraData->GetCharacter(index);
}

world::character::Character *GroupCore::GetLeader() const 
{
    return extraData->GetLeader();
}

world::WorldTile *GroupCore::GetFinalDestination() const
{
    return travelActionData->IsOnRoute == true ? travelActionData->Route[travelActionData->PlannedDestinationCount - 1] : nullptr;
}

float GroupCore::GetTravelProgress() const
{
    return (float)travelActionData->Progress / (float)travelActionData->Duration;
}

GroupClasses GroupCore::GetClass() const
{
    return type->Class;
}

GroupCore *GroupCore::GetOther()
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

int GroupCore::GetCarryCapacity() const
{
    if(extraData != nullptr)
    {
        return extraData->GetCarryCapacity();
    }

    return 50 + 10 * level;
}

int GroupCore::GetCarriedWeight() const 
{
    if(extraData != nullptr)
        return extraData->GetCarriedWeight();

    return 30;
}

SkillData GroupCore::GetMostSkilledMember(character::SkillTypes skillType) const
{
    if(extraData != nullptr)
    {
        return extraData->GetMostSkilledMember(skillType);
    }

    return {nullptr, level * 3};
}

SkillData GroupCore::GetLeastSkilledMember(character::SkillTypes skillType) const
{
    if(extraData != nullptr)
    {
        return extraData->GetLeastSkilledMember(skillType);
    }

    return {nullptr, 0};
}

bool GroupCore::IsEncumbered() const
{
    if(extraData != nullptr)
    {
        return extraData->IsEncumbered();
    }

    return false;
}

void GroupCore::Update()
{
    if(extraData != nullptr)
    {
        extraData->Update();
        return;
    }

    if(conditionFactor != 0)
    {
        if(WorldScene::Get()->GetTime().TotalHourCount - conditionTimestamp > 24)
        {
            conditionTimestamp = WorldScene::Get()->GetTime().TotalHourCount;

            if(conditionFactor > 0)
            {
                conditionFactor--;
            }
            else
            {
                conditionFactor++;
            }
        }
    }
}

void GroupCore::CheckFatigue()
{
    if(extraData != nullptr)
    {
        extraData->CheckFatigue();
        return;
    }

    if(this->IsDoing(GroupActions::TAKE_LONG_REST) == true)
        return;

    timeSinceLongRest += 1;

    static const auto FATIGUE_ONSET_SINCE_REST = engine::ConfigManager::Get()->GetValue(game::ConfigValues::FATIGUE_ONSET_SINCE_REST).Integer;

    static const auto TIME_UNTIL_FATIGUE_CHECK = FATIGUE_ONSET_SINCE_REST * WorldTime::HOUR_SIZE;

    if(timeSinceLongRest < TIME_UNTIL_FATIGUE_CHECK || (timeSinceLongRest - TIME_UNTIL_FATIGUE_CHECK) % WorldTime::HOUR_SIZE != 0)
    {
        return;
    }

    auto bonus = level;
    auto savingThrow = utility::RollD20Dice(FATIGUE_SAVING_THROW_DC, bonus);
    if(savingThrow.IsAnyFailure())
    {
        isFatigued = true;
    }
}

void GroupCore::FinishLongRest()
{
    if(extraData != nullptr)
    {
        extraData->FinishLongRest();
        return;
    }

    timeSinceLongRest = 0;

    isFatigued = false;
}

void GroupCore::DetermineAction()
{
    if(extraData != nullptr)
    {
        extraData->DetermineAction();
        return;
    }

    MachineMind::Get()->DetermineAction(*this);
}

bool GroupCore::ValidateAction(GroupActions actionType, const GroupActionData &actionData)
{
    auto possibleAction = GroupActionFactory::Get()->BuildAction(actionType);

    return possibleAction->CanPerform(*this, actionData);
}   

GroupActionResult GroupCore::SelectAction(GroupActions actionType, const GroupActionData &actionData)
{
    if(action && action->Type == actionType)
        return {};

    action = GroupActionFactory::Get()->BuildAction(actionType);

    actionProgress = 0;

    travelActionData->Intensity = ActionIntensities::NORMAL;

    auto result = action->Initiate(*this, actionData);
    result.Content.isAlreadyEngaged = actionData.IsEngaged;

    if(extraData != nullptr)
    {
        extraData->HandleActionSelection(result);
    }

    return result;
}

void GroupCore::PerformAction()
{
    CheckFatigue();

    if(action)
    {
        actionProgress += GetProgressRate();

        auto result = action->Perform(*this);

        if(extraData != nullptr)
        {
            extraData->HandleActionPerformance(result);
        }
    }
}

void GroupCore::CancelAction()
{
    action = nullptr;

    actionProgress = 0;
}

void GroupCore::IntensifyAction()
{
    if(action == nullptr)
        return;
        
    if(action->HasVaryingIntensity == false)
        return;

    switch(travelActionData->Intensity)
    {
        case ActionIntensities::LEISURELY:
            travelActionData->Intensity = ActionIntensities::NORMAL;
            break;
        case ActionIntensities::NORMAL:
            travelActionData->Intensity = ActionIntensities::INTENSE;
            break;
    }
}

void GroupCore::SlackenAction()
{
    if(action == nullptr)
        return;

    if(action->HasVaryingIntensity == false)
        return;
        
    switch(travelActionData->Intensity)
    {
        case ActionIntensities::NORMAL:
            travelActionData->Intensity = ActionIntensities::LEISURELY;
            break;
        case ActionIntensities::INTENSE:
            travelActionData->Intensity = ActionIntensities::NORMAL;
            break;
    }
}

void GroupCore::EngageGroup(Encounter *_encounter)
{
    encounter = _encounter;

    this->SelectAction(GroupActions::ENGAGE, {false});
}

void GroupCore::ExitBattle()
{
    encounter = nullptr;

    CancelAction();
}

void GroupCore::AddItem(character::ItemTypes type, int amount)
{
    if(extraData != nullptr)
    {
        extraData->AddItem(type, amount);
        return;
    }

    itemValue += character::ItemFactory::Get()->CreateType(type)->Value * amount;
}

void GroupCore::RemoveItem(character::Item *item)
{
    if(extraData != nullptr)
    {
        extraData->RemoveItem(item);
        return;
    }

    itemValue -= item->Type->Value;
}

void GroupCore::RemoveItemAmount(character::ItemTypes type, int amount)
{
    if(extraData != nullptr)
    {
        extraData->RemoveItemAmount(type, amount);
        return;
    }

    itemValue -= character::ItemFactory::Get()->CreateType(type)->Value * amount;
}

void GroupCore::SetTile(WorldTile *newTile)
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

float GroupCore::GetActionProgress() const
{
    if(action->HasVaryingIntensity == false)
    {
        return (float)actionProgress / (float)action->GetDuration(*this);   
    }
    else
    {
        return (float)travelActionData->Progress / (float)travelActionData->Duration;   
    }
}

int GroupCore::GetRemainingActionDuration() const
{
    if(action->HasVaryingIntensity == false)
    {
        return action->GetDuration(*this) - actionProgress;
    }
    else
    {
        return action->GetDuration(*this) - travelActionData->Progress;
    }
}

int GroupCore::GetProgressRate() const
{
    if(action->HasVaryingIntensity == false)
    {
        return GroupAction::BASE_PROGRESS_RATE;
    }

    auto encumbrancePenalty = IsEncumbered() == true ? GroupAction::BASE_ENCUMBRANCE_PENALTY : 0;

    switch(travelActionData->Intensity)
    {
        case ActionIntensities::LEISURELY:
            return GroupAction::BASE_PROGRESS_RATE / 2;
        case ActionIntensities::NORMAL:
            return GroupAction::BASE_PROGRESS_RATE - encumbrancePenalty;
        case ActionIntensities::INTENSE:
            return GroupAction::BASE_PROGRESS_RATE + (GroupAction::BASE_PROGRESS_RATE / 2) - encumbrancePenalty;
    }
}

world::character::Item *GroupCore::GetItem(int index)
{
    if(extraData != nullptr)
    {
        return extraData->GetItem(index);
    }
}

int GroupCore::GetItemAmount(character::ItemTypes type)
{
    if(extraData != nullptr)
    {
        return extraData->GetItemAmount(type);
    }

    auto amount = itemValue / character::ItemFactory::Get()->CreateType(type)->Value;
    return amount;
}

const container::Pool <world::character::Item> &GroupCore::GetItems() const
{
    if(extraData != nullptr)
    {
        return extraData->GetItems();
    }
}

world::settlement::Settlement *GroupCore::GetCurrentSettlement()
{
    if(IsDoing(GroupActions::TRAVEL))
        return nullptr;

    return tile->GetSettlement();
}

int GroupCore::GetDistanceTo(const GroupCore *group) const
{
    return tile->GetDistanceTo(*group->GetTile());
}

int GroupCore::GetMuleCount() const 
{
    if(extraData != nullptr)
    {
        return extraData->GetMuleCount();
    }

    return 5;
}

void GroupCore::SetMuleCount(int amount)
{
    if(extraData != nullptr)
    {
        return extraData->SetMuleCount(amount);
    }
}

int GroupCore::GetFoodConsumption() const
{
    if(extraData != nullptr)
    {
        return extraData->GetFoodConsumption();
    }

    return characterHandler.characterCount + GetMuleCount() * MULE_FOOD_CONSUMPTION;
}

bool GroupCore::DoesRulePolity() const
{
    if(extraData != nullptr)
    {
        return extraData->DoesRulePolity();
    }

    return false;
}

world::polity::Polity *GroupCore::GetDomain() 
{
    if(extraData != nullptr)
    {
        return extraData->GetDomain();
    }

    return nullptr;
}

void GroupCore::SetDomain(polity::Polity *polity)
{
    if(extraData != nullptr)
    {
        extraData->SetDomain(polity);
    }
}

int GroupCore::GetDomainSettlementCount() const
{
    if(extraData != nullptr)
    {
        return extraData->GetDomainSettlementCount();
    }

    return 0;
}

bool GroupCore::HasSameAllegiance(const GroupCore &other) const
{
    return GetHome()->GetPolity() == other.GetHome()->GetPolity();
}

int GroupCore::GetSize() const 
{
    if(extraData != nullptr)
    {
        return extraData->GetSize();
    }

    return characterHandler.characterCount;
}

void GroupCore::GainExperience(int experience)
{
    if(extraData != nullptr)
    {
        extraData->GainExperience(experience);
        return;
    }

    for(CharacterEssence &character : *characterHandler.characters)
    {
        character.GainExperience(experience);
    }
}

int GroupCore::GetLevel() const
{
    if(extraData != nullptr)
    {
        return extraData->GetLevel();
    }

    auto level = 0;
    for(auto &character : *characterHandler.characters)
    {
        level += character.level;
    }

    return level / characterHandler.characterCount;
}

Attitudes GroupCore::GetAttitude() const
{
    if(extraData != nullptr)
    {
        return extraData->GetAttitude();
    }

    return Attitudes::INDIFFERENT;
}

void GroupCore::SetAttitude(Attitudes attitude)
{
    if(extraData != nullptr)
    {
        extraData->SetAttitude(attitude);
        return;
    }
}

void GroupCore::AddPositiveCondition(int strength)
{
    conditionFactor += strength;

    conditionTimestamp = WorldScene::Get()->GetTime().TotalHourCount;
}

void GroupCore::AddNegativeCondition(int strength)
{
    conditionFactor -= strength;

    conditionTimestamp = WorldScene::Get()->GetTime().TotalHourCount;
}

Word GroupCore::GetLeaderName() const
{
    if(extraData != nullptr)
    {
        return extraData->GetLeader()->GetName();
    }

    return world::character::NameGenerator::Get()->FetchName(uniqueId);
}