#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenBattle/World/Character/Condition.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/Battle/Combatant.h"

using namespace world::character;

Condition::Condition() {duration = 0;}

Condition::Condition(ConditionTypes _type, battle::Combatant * _combatant, Integer _difficultyClass) : combatant(_combatant)
{
    type = _type;
    difficultyClass = _difficultyClass;

    switch(type)
    {
        case ConditionTypes::CHILL:
            InitializeCold();
            break;
        case ConditionTypes::EVASION:
            InitializeEvasion();
            break;
        default:
            break;
    }
}

void Condition::Apply()
{
    if(IsActive() == false)
        return;

    duration--;
    if(duration == 0)
    {
        combatant->conditions.Remove(this);
    }

    switch(type)
    {
        case ConditionTypes::CHILL:
            ApplyCold();
            break;
        case ConditionTypes::EVASION:
            ApplyEvasion();
            break;
        default:
            break;
    }
}

void Condition::InitializeCold() 
{
    duration = 3;
}

void Condition::ApplyCold() 
{   
    auto roll = utility::GetRandom(1, 20);
    if(roll == 20)
    {
        if(duration != 0)
        {
            combatant->conditions.Remove(this);
            duration = 0;
        }
        return;
    }
    else if(roll == 1)
    {
        combatant->speedPenalty += 4;
    }
    else
    {
        auto savingThrow = roll + combatant->character->GetFortitudeSaveBonus();
        if(savingThrow > difficultyClass)
        {
            combatant->speedPenalty += 0;
        }
        else
        {
            combatant->speedPenalty += 2;
        }
    }
}

void Condition::InitializeEvasion()
{
    duration = 1;
}

void Condition::ApplyEvasion() 
{   
}