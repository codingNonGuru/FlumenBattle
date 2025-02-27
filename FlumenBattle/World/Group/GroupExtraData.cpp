#include <type_traits>

#include "GroupExtraData.h"
#include "GroupCore.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/World/Character/CharacterClass.h"
#include "FlumenBattle/World/Tile/WorldTile.h"
#include "FlumenBattle/World/Group/GroupAction.h"
#include "FlumenBattle/World/Group/GroupBatch.h"
#include "FlumenBattle/World/Group/GroupType.h"
#include "FlumenBattle/World/Group/Encounter.h"
#include "FlumenBattle/World/Group/GroupMind.h"
#include "FlumenBattle/World/Group/GroupActionFactory.h"
#include "FlumenBattle/World/Group/MachineMind.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Polity/Polity.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/Config.h"

using namespace world::group;

#define FATIGUE_SAVING_THROW_DC 12

#define MULE_CARRY_CAPACITY 25

#define MULE_FOOD_CONSUMPTION 3

void GroupExtraData::Initialize(GroupCore *newGroupCore)
{
    this->groupCore = newGroupCore;

    this->controller = groupCore->type->Controller;

    domain = nullptr;

    leader = nullptr;
    auto charismaModifier = -100;
    for(auto &character : characters)
    {
        auto modifier = character.GetAbility(world::character::AbilityTypes::CHARISMA).Modifier;
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

bool GroupExtraData::IsAlive()
{
    for(auto &character : characters)
    {
        if(character.IsAlive() == true)
        {
            return true;
        }
    }

    return false;
}

int GroupExtraData::GetLivingCount() const
{
    auto count = 0;
    for(auto &character : characters)
    {
        if(character.IsAlive() == true)
            count++;
    }

    return count;
}

world::character::Character *GroupExtraData::GetCharacter(int index) 
{
    return characters.Get(index);
}

int GroupExtraData::GetCarriedWeight() const
{
    return items.GetTotalWeight();
}

int GroupExtraData::GetCarryCapacity() const
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

SkillData GroupExtraData::GetMostSkilledMember(character::SkillTypes skill) const
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

SkillData GroupExtraData::GetLeastSkilledMember(character::SkillTypes skill) const
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

bool GroupExtraData::IsEncumbered() const
{
    return GetCarriedWeight() > GetCarryCapacity();
}

void GroupExtraData::Update()
{
    for(auto &character : characters)
    {
        character.Update();
    }
}

void GroupExtraData::CheckFatigue()
{
    if(groupCore->IsDoing(GroupActions::TAKE_LONG_REST) == true)
        return;

    groupCore->timeSinceLongRest += 1;

    static const auto FATIGUE_ONSET_SINCE_REST = engine::ConfigManager::Get()->GetValue(game::ConfigValues::FATIGUE_ONSET_SINCE_REST).Integer;

    static const auto TIME_UNTIL_FATIGUE_CHECK = FATIGUE_ONSET_SINCE_REST * WorldTime::HOUR_SIZE;

    if(groupCore->timeSinceLongRest < TIME_UNTIL_FATIGUE_CHECK || (groupCore->timeSinceLongRest - TIME_UNTIL_FATIGUE_CHECK) % WorldTime::HOUR_SIZE != 0)
    {
        return;
    }

    for(auto &character : characters)
    {
        auto bonus = character.GetFortitudeSaveBonus();
        auto savingThrow = utility::RollD20Dice(FATIGUE_SAVING_THROW_DC, bonus);
        if(savingThrow.IsAnyFailure())
        {
            character.AddCondition({character::Conditions::FATIGUE});
        }
    }
}

void GroupExtraData::FinishLongRest()
{
    for(auto &character : characters)
    {
        character.TakeLongRest();
    }

    groupCore->timeSinceLongRest = 0;
}

void GroupExtraData::DetermineAction()
{
    controller->DetermineAction(*groupCore);
}

void GroupExtraData::AddItem(character::ItemTypes type, int amount)
{
    items.Add(type, amount);
}

void GroupExtraData::RemoveItem(character::Item *item)
{
    items.Remove(item);
}

void GroupExtraData::RemoveItemAmount(character::ItemTypes type, int amount)
{
    items.RemoveAmount(type, amount);
}

world::character::Item *GroupExtraData::GetItem(int index)
{
    return items.GetItem(index);
}

int GroupExtraData::GetItemAmount(character::ItemTypes type)
{
    return items.GetAmount(type);
}

const container::Pool <world::character::Item> &GroupExtraData::GetItems() const
{
    return items.GetItems();
}

int GroupExtraData::GetFoodConsumption() const
{
    return characters.GetSize() + muleCount * MULE_FOOD_CONSUMPTION;
}

void GroupExtraData::SetDomain(polity::Polity *polity)
{
    domain = polity;
}

int GroupExtraData::GetDomainSettlementCount() const
{
    return domain->GetSettlements().GetSize();
}

int GroupExtraData::GetSize() const 
{
    return characters.GetSize();
}

void GroupExtraData::GainExperience(int experience)
{
    for(auto &character : characters)
    {
        character.GainExperience(experience);
    }
}

int GroupExtraData::GetLevel() const
{
    auto level = 0;
    for(auto &character : characters)
    {
        level += character.GetLevel();
    }

    return level / characters.GetSize();
}

bool GroupExtraData::DoesRulePolity() const
{
    return domain != nullptr;
}

Attitudes GroupExtraData::GetAttitude() const
{
    return attitude;
}

void GroupExtraData::SetMuleCount(int amount)
{
    muleCount = amount;
}

void GroupExtraData::HandleActionSelection(const GroupActionResult &result)
{
    controller->RegisterActionInitiation(*groupCore, result);
}

void GroupExtraData::HandleActionPerformance(const GroupActionResult &result)
{
    controller->RegisterActionPerformance(*groupCore, result);
}