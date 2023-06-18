#include "GroupDynamics.h"
#include "FlumenBattle/World/Settlement/Settlement.h"

using namespace world;
using namespace world::group;

#define ARMOR_CLASS 15

GroupDynamics::GroupDynamics()
{
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
}

void GroupDynamics::StrengthenPatrol()
{
    patrolStrength++;
    if(patrolStrength > 10)
    {
        patrolStrength = 10;
    }
}