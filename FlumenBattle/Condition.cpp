#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenBattle/Condition.h"
#include "FlumenBattle/Character.h"

Condition::Condition() {duration = 0;}

Condition::Condition(ConditionTypes _type, Character * _character, Integer _difficultyClass) : character(_character)
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
        character->conditions.Remove(this);
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
    auto savingThrow = utility::GetRandom(1, 20) + character->constitution.Modifier;
    if(savingThrow > difficultyClass)
    {
        if(duration != 0)
        {
            character->conditions.Remove(this);
            duration = 0;
        }
        return;
    }

    character->speedPenalty += 3;
}

void Condition::InitializeEvasion()
{
    duration = 1;
}

void Condition::ApplyEvasion() 
{   
}