#include "GroupDynamics.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Group/GroupFactory.h"
#include "FlumenBattle/World/Group/GroupEssence.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Group/Types.h"
#include "FlumenBattle/World/WorldTime.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Group/GroupAllocator.h"
#include "FlumenBattle/Race.h"

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

#define PATROL_CAP 4

GroupDynamics::GroupDynamics() {}

void GroupDynamics::Initialize(settlement::Settlement &settlement)
{
    lastSpawnTime = -1;
}

void GroupDynamics::Update(settlement::Settlement &settlement)
{
    UpdateSimulationLevel(settlement);

    AddAdventurer(settlement);

    AddMerchant(settlement);

    AddBandit(settlement);

    AddGarrison(settlement);

    AddRaider(settlement);

    const auto simulationLevel = settlement.GetSimulationLevel();
    if(simulationLevel != SimulationLevels::ADVANCED)
    {
        UpdateEncounters(settlement);
    }
}

void GroupDynamics::UpdateEncounters(settlement::Settlement &settlement)
{
    bool thereIsNoOneToFight = GetBanditStrength() == 0 || GetPatrolStrength() == 0;
    if(thereIsNoOneToFight == true)
        return;
    
    if(utility::RollD100Dice() <= ENCOUNTER_CHANCE_DC)
        return;
        
    auto banditAttackRoll = utility::RollD20Dice() + GetBanditStrength();

    auto bonus = settlement.GetModifier(settlement::Modifiers::PATROL_ATTACK_ROLLS);
    auto patrolAttackRoll = utility::RollD20Dice() + GetPatrolStrength() + bonus;

    if(banditAttackRoll >= ARMOR_CLASS)
    {
        auto patrol = patrols.GetRandom();

        patrols.RemoveAt(patrol);
    }

    if(patrolAttackRoll >= ARMOR_CLASS)
    {
        auto bandit = bandits.GetRandom();

        bandits.RemoveAt(bandit);
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
            adventurer.Group = group::GroupFactory::CreateMachineGroup({group::GroupClasses::ADVENTURER, settlement.GetRace()->Type, &settlement});
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
            merchant.Group = group::GroupFactory::CreateMachineGroup({group::GroupClasses::MERCHANT, settlement.GetRace()->Type, &settlement});
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
            bandit.Group = group::GroupFactory::CreateMachineGroup({group::GroupClasses::BANDIT, settlement.GetRace()->Type, &settlement});
        }
        else if(bandit.Group != nullptr && simulationLevel != SimulationLevels::ADVANCED)
        {
            group::GroupAllocator::Get()->Free(bandit.Group, false);

            bandit.Group = nullptr;
        }
    }

    for(auto &patrol : patrols)
    {
        if(patrol.Group == nullptr && simulationLevel == SimulationLevels::ADVANCED)
        {
            patrol.Group = group::GroupFactory::CreateMachineGroup({group::GroupClasses::PATROL, settlement.GetRace()->Type, &settlement});
        }
        else if(patrol.Group != nullptr && simulationLevel != SimulationLevels::ADVANCED)
        {
            group::GroupAllocator::Get()->Free(patrol.Group, false);

            patrol.Group = nullptr;
        }
    }

    for(auto &garrison : garrisons)
    {
        if(garrison.Group == nullptr && simulationLevel == SimulationLevels::ADVANCED)
        {
            garrison.Group = group::GroupFactory::CreateMachineGroup({group::GroupClasses::GARRISON, settlement.GetRace()->Type, &settlement});
        }
        else if(garrison.Group != nullptr && simulationLevel != SimulationLevels::ADVANCED)
        {
            group::GroupAllocator::Get()->Free(garrison.Group, false);

            garrison.Group = nullptr;
        }
    }

    for(auto &raider : raiders)
    {
        if(raider.Group == nullptr && simulationLevel == SimulationLevels::ADVANCED)
        {
            raider.Group = group::GroupFactory::CreateMachineGroup({group::GroupClasses::RAIDER, settlement.GetRace()->Type, &settlement});
        }
        else if(raider.Group != nullptr && simulationLevel != SimulationLevels::ADVANCED)
        {
            group::GroupAllocator::Get()->Free(raider.Group, false);

            raider.Group = nullptr;
        }
    }
}

void GroupDynamics::AddPatrol(settlement::Settlement &settlement)
{
    if(GetPatrolStrength() == PATROL_CAP)
        return;

    const auto simulationLevel = settlement.GetSimulationLevel();
    if(simulationLevel == SimulationLevels::BASIC || simulationLevel == SimulationLevels::MEDIUM)
    {
        *patrols.Add() = {nullptr};
    }
    else
    {
        auto patrol = group::GroupFactory::CreateMachineGroup({group::GroupClasses::PATROL, settlement.GetRace()->Type, &settlement});
        *patrols.Add() = {patrol};
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
        auto adventurer = group::GroupFactory::CreateMachineGroup({group::GroupClasses::ADVENTURER, settlement.GetRace()->Type, &settlement});
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
        auto merchant = group::GroupFactory::CreateMachineGroup({group::GroupClasses::MERCHANT, settlement.GetRace()->Type, &settlement});
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
            auto bandit = group::GroupFactory::CreateMachineGroup({group::GroupClasses::BANDIT, settlement.GetRace()->Type, &settlement});
            *bandits.Add() = {bandit};
        }
    }
}

void GroupDynamics::AddGarrison(settlement::Settlement &settlement)
{
    if(garrisons.GetSize() == 1)
        return;

    const auto simulationLevel = settlement.GetSimulationLevel();
    if(simulationLevel == SimulationLevels::ADVANCED)
    {
        auto garrison = group::GroupFactory::CreateMachineGroup({group::GroupClasses::GARRISON, settlement.GetRace()->Type, &settlement});
        *garrisons.Add() = {garrison};
    }
    else
    {
        *garrisons.Add() = {nullptr};
    }
}

void GroupDynamics::AddRaider(settlement::Settlement &settlement)
{
    if(raiders.GetSize() == 1)
        return;

    const auto simulationLevel = settlement.GetSimulationLevel();
    if(simulationLevel == SimulationLevels::ADVANCED)
    {
        auto raider = group::GroupFactory::CreateMachineGroup({group::GroupClasses::RAIDER, settlement.GetRace()->Type, &settlement});
        *raiders.Add() = {raider};
    }
    else
    {
        *raiders.Add() = {nullptr};
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
    else if(group.GetClass() == group::GroupClasses::PATROL)
    {
        patrols.Remove(&group);
    }
    else if(group.GetClass() == group::GroupClasses::GARRISON)
    {
        garrisons.Remove(&group);
    }
    else if(group.GetClass() == group::GroupClasses::RAIDER)
    {
        raiders.Remove(&group);
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

int GroupDynamics::GetPatrolStrength() const
{
    return patrols.GetSize();
}

int GroupDynamics::GetGarrisonStrength() const
{
    return garrisons.GetSize();
}

int GroupDynamics::GetRaiderStrength() const
{
    return raiders.GetSize();
}