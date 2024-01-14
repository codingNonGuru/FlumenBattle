#include "GroupDynamics.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Group/GroupFactory.h"
#include "FlumenBattle/World/Group/GroupEssence.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Group/Types.h"
#include "FlumenBattle/World/WorldTime.h"
#include "FlumenBattle/World/WorldScene.h"

using namespace world;
using namespace world::group;

#define ARMOR_CLASS 15

#define ADVENTURER_CAP 2

#define ADVENTURER_SPAWN_TIME 8

#define MERCHANT_CAP 1

#define MERCHANT_SPAWN_TIME 4

GroupDynamics::GroupDynamics() {}

void GroupDynamics::Initialize()
{
    lastSpawnTime = -1;

    banditStrength = 5;

    patrolStrength = 5;

    merchantStrength = 3;
}

void GroupDynamics::Update(settlement::Settlement &settlement)
{
    if(banditStrength > 0 && patrolStrength > 0)
    {
        auto banditAttackRoll = utility::GetRandom(1, 20) + banditStrength;

        auto bonus = settlement.GetModifier(settlement::Modifiers::PATROL_ATTACK_ROLLS);
        auto patrolAttackRoll = utility::GetRandom(1, 20) + patrolStrength + bonus;

        if(banditAttackRoll >= ARMOR_CLASS)
        {
            patrolStrength--;
        }

        if(patrolAttackRoll >= ARMOR_CLASS)
        {
            banditStrength--;
        }
    }

    auto banditSpawnPenalty = banditStrength >= 7 ? 1 : 0;
    auto banditSpawnBonus = settlement.GetPopulation() >= 10 ? 2 : (settlement.GetPopulation() >= 5 ? 1 : 0);
    auto diceRoll = utility::GetRandom(1, 100) + banditSpawnBonus - banditSpawnPenalty;
    if(diceRoll <= 5)
    {
        banditStrength++;
        if(banditStrength > 10)
        {
            banditStrength = 10;
        }
    }

    AddAdventurer(settlement);

    AddMerchant(settlement);
}

void GroupDynamics::StrengthenPatrol()
{
    patrolStrength++;
    if(patrolStrength > 10)
    {
        patrolStrength = 10;
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

    auto simulationLevel = settlement.GetSimulationLevel();
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

    auto time = world::WorldScene::Get()->GetTime().TotalHourCount;
    if(time - lastSpawnTime < MERCHANT_SPAWN_TIME)
    {
        return;
    }

    lastSpawnTime = time;

    auto simulationLevel = settlement.GetSimulationLevel();
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
}

int GroupDynamics::GetAdventurerStrength()
{
    return adventurers.GetSize();
}

int GroupDynamics::GetMerchantStrength()
{
    return merchants.GetSize();
}