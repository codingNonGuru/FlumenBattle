#include "GroupDynamics.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Group/GroupFactory.h"
#include "FlumenBattle/World/Group/GroupEssence.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Group/Types.h"
#include "FlumenBattle/World/WorldTime.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Group/GroupAllocator.h"

using namespace world;
using namespace world::group;

#define ARMOR_CLASS 15

#define ADVENTURER_CAP 2

#define ADVENTURER_SPAWN_TIME 8

#define MERCHANT_CAP 1

#define MERCHANT_SPAWN_TIME 4

#define BANDIT_CAP 4

#define BANDIT_SPAWN_DC 6

#define ENCOUNTER_CHANCE_DC 95

GroupDynamics::GroupDynamics() {}

void GroupDynamics::Initialize()
{
    lastSpawnTime = -1;

    patrolStrength = 5;
}

void GroupDynamics::Update(settlement::Settlement &settlement)
{
    UpdateSimulationLevel(settlement);

    AddAdventurer(settlement);

    AddMerchant(settlement);

    AddBandit(settlement);

    const auto simulationLevel = settlement.GetSimulationLevel();
    if(simulationLevel != SimulationLevels::ADVANCED)
    {
        UpdateEncounters(settlement);
    }
}

void GroupDynamics::UpdateEncounters(settlement::Settlement &settlement)
{
    bool thereIsNoOneToFight = GetBanditStrength() == 0 || patrolStrength == 0;
    if(thereIsNoOneToFight == true)
        return;
    
    if(utility::RollD100Dice() <= ENCOUNTER_CHANCE_DC)
        return;
        
    auto banditAttackRoll = utility::GetRandom(1, 20) + GetBanditStrength();

    auto bonus = settlement.GetModifier(settlement::Modifiers::PATROL_ATTACK_ROLLS);
    auto patrolAttackRoll = utility::GetRandom(1, 20) + patrolStrength + bonus;

    if(banditAttackRoll >= ARMOR_CLASS)
    {
        patrolStrength--;
    }

    if(patrolAttackRoll >= ARMOR_CLASS)
    {
        auto bandit = bandits.GetRandom();

        //group::GroupAllocator::Get()->Free(bandit->Group, true);
        bandits.RemoveAt(bandit);
    }
}

void GroupDynamics::StrengthenPatrol()
{
    patrolStrength++;
    if(patrolStrength > 10)
    {
        patrolStrength = 10;
    }
}

void GroupDynamics::UpdateSimulationLevel(settlement::Settlement &settlement)
{
    if(settlement.HasUsedSimulationChange() == true)
        return;

    settlement.UseSimulationChange();

    const auto simulationLevel = settlement.GetSimulationLevel();

    for(auto &adventurer : adventurers)
    {
        if(adventurer.Group == nullptr && simulationLevel == SimulationLevels::ADVANCED)
        {
            adventurer.Group = group::GroupFactory::Create({group::GroupClasses::ADVENTURER, RaceTypes::ORC, &settlement});
        }
        else if(adventurer.Group != nullptr && simulationLevel != SimulationLevels::ADVANCED)
        {
            group::GroupAllocator::Get()->Free(adventurer.Group, false);

            adventurer.Group = nullptr;
        }
    }

    for(auto &merchant : merchants)
    {
        if(merchant.Group == nullptr && simulationLevel != SimulationLevels::BASIC)
        {
            merchant.Group = group::GroupFactory::Create({group::GroupClasses::MERCHANT, RaceTypes::ORC, &settlement});
        }
        else if(merchant.Group != nullptr && simulationLevel == SimulationLevels::BASIC)
        {
            group::GroupAllocator::Get()->Free(merchant.Group, false);

            merchant.Group = nullptr;
        }
    }

    for(auto &bandit : bandits)
    {
        if(bandit.Group == nullptr && simulationLevel == SimulationLevels::ADVANCED)
        {
            bandit.Group = group::GroupFactory::Create({group::GroupClasses::BANDIT, RaceTypes::ORC, &settlement});
        }
        else if(bandit.Group != nullptr && simulationLevel != SimulationLevels::ADVANCED)
        {
            group::GroupAllocator::Get()->Free(bandit.Group, false);

            bandit.Group = nullptr;
        }
    }
}

void GroupDynamics::AddAdventurer(settlement::Settlement &settlement)
{
    if(GetAdventurerStrength() == ADVENTURER_CAP)
    {
        return;
    }

    auto time = world::WorldScene::Get()->GetTime().TotalHourCount;
    if(time - lastSpawnTime < ADVENTURER_SPAWN_TIME)
    {
        return;
    }

    lastSpawnTime = time;

    const auto simulationLevel = settlement.GetSimulationLevel();
    if(simulationLevel == SimulationLevels::BASIC || simulationLevel == SimulationLevels::MEDIUM)
    {
        *adventurers.Add() = {nullptr};
    }
    else
    {
        auto adventurer = group::GroupFactory::Create({group::GroupClasses::ADVENTURER, RaceTypes::ORC, &settlement});
        *adventurers.Add() = {adventurer};
    }
}

void GroupDynamics::AddMerchant(settlement::Settlement &settlement)
{
    if(GetMerchantStrength() == MERCHANT_CAP)
    {
        return;
    }

    const auto time = world::WorldScene::Get()->GetTime().TotalHourCount;
    if(time - lastSpawnTime < MERCHANT_SPAWN_TIME)
    {
        return;
    }

    lastSpawnTime = time;

    const auto simulationLevel = settlement.GetSimulationLevel();
    if(simulationLevel == SimulationLevels::BASIC)
    {
        *merchants.Add() = {nullptr};
    }
    else
    {
        auto merchant = group::GroupFactory::Create({group::GroupClasses::MERCHANT, RaceTypes::ORC, &settlement});
        *merchants.Add() = {merchant};
    }
}

void GroupDynamics::AddBandit(settlement::Settlement &settlement)
{
    if(GetBanditStrength() == BANDIT_CAP)
        return;

    auto banditSpawnPenalty = GetBanditStrength() > 3 ? 1 : 0;
    auto banditSpawnBonus = settlement.GetPopulation() >= 10 ? 2 : (settlement.GetPopulation() >= 5 ? 1 : 0);

    auto diceRoll = utility::RollD100Dice() + banditSpawnBonus - banditSpawnPenalty;
    if(diceRoll <= BANDIT_SPAWN_DC)
    {
        const auto simulationLevel = settlement.GetSimulationLevel();
        if(simulationLevel == SimulationLevels::BASIC || simulationLevel == SimulationLevels::MEDIUM)
        {
            *bandits.Add() = {nullptr};
        }
        else
        {
            auto bandit = group::GroupFactory::Create({group::GroupClasses::BANDIT, RaceTypes::ORC, &settlement});
            *bandits.Add() = {bandit};
        }
    }
}

void GroupDynamics::RemoveGroup(const group::Group &group)
{
    if(group.GetClass() == group::GroupClasses::ADVENTURER)
    {
        adventurers.Remove(&group);
    }
    else if(group.GetClass() == group::GroupClasses::MERCHANT)
    {
        merchants.Remove(&group);
    }
    else if(group.GetClass() == group::GroupClasses::BANDIT)
    {
        bandits.Remove(&group);
    }
}

int GroupDynamics::GetAdventurerStrength()
{
    return adventurers.GetSize();
}

int GroupDynamics::GetMerchantStrength()
{
    return merchants.GetSize();
}

int GroupDynamics::GetBanditStrength() const
{
    return bandits.GetSize();
}