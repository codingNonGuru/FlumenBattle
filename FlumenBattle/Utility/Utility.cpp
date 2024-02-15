#include <iostream>

#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenBattle/Utility/Utility.h"
#include "FlumenBattle/World/Settlement/Types.h"
#include "FlumenBattle/World/Group/Types.h"

utility::Result utility::RollD4Dice()
{
    return utility::GetRandom(1, 4);
}

utility::Result utility::RollD6Dice()
{
    return utility::GetRandom(1, 6);
}

utility::Result utility::RollD8Dice()
{
    return utility::GetRandom(1, 8);
}

utility::Result utility::RollD10Dice()
{
    return utility::GetRandom(1, 10);
}

utility::Result utility::RollD12Dice()
{
    return utility::GetRandom(1, 12);
}

utility::Result utility::RollD20Dice()
{
    return utility::GetRandom(1, 20);
}

utility::Result utility::RollDice(utility::RollMaterial material)
{
    auto result = 0;

    for(int i = 0; i < material.DieCount; ++i)
    {
        switch(material.Die)
        {
        case RollDies::D4:
            result += RollD4Dice();
            break;
        case RollDies::D6:
            result += RollD6Dice();
            break;
        case RollDies::D8:
            result += RollD8Dice();
            break;
        case RollDies::D10:
            result += RollD10Dice();
            break;
        case RollDies::D12:
            result += RollD12Dice();
            break;
        case RollDies::D20:
            result += RollD20Dice();
            break;
        }
    }

    result += material.Bonus;

    return result;
}

utility::Result utility::RollD100Dice()
{
    return utility::GetRandom(1, 100);
}

utility::Success utility::RollD20Dice(DifficultyClass difficultyClass, Bonus bonus)
{
    auto diceRoll = utility::RollD20Dice();
    auto compoundedRoll = diceRoll + bonus;
    auto difference = compoundedRoll - difficultyClass;
    auto isSuccess = difference >= 0;

    auto success = SuccessTypes();

    if(difference >= 10)
    {
        success = SuccessTypes::CRITICAL_SUCCESS;

        if(diceRoll == 1)
        {
            success = SuccessTypes::SUCCESS;
        }
    }
    else if(isSuccess == true)
    {
        success = SuccessTypes::SUCCESS;

        if(diceRoll == 1)
        {
            success = SuccessTypes::FAILURE;
        }
        else if(diceRoll == 20)
        {
            success = SuccessTypes::CRITICAL_SUCCESS;
        }
    }
    else if(difference > -10)
    {
        success = SuccessTypes::FAILURE;

        if(diceRoll == 1)
        {
            success = SuccessTypes::CRITICAL_FAILURE;
        }
        else if(diceRoll == 20)
        {
            success = SuccessTypes::SUCCESS;
        }
    }
    else
    {
        success = SuccessTypes::CRITICAL_FAILURE;

        if(diceRoll == 20)
        {
            success = SuccessTypes::FAILURE;
        }
    }

    return {success, diceRoll, bonus, difficultyClass};
}

const char *utility::GetAttitudeName(world::settlement::SettlementAttitudes attitude)
{
    switch(attitude)
    {
    case world::settlement::SettlementAttitudes::HOSTILE:
        return "Hostile";
    case world::settlement::SettlementAttitudes::UNFRIENDLY:
        return "Unfriendly";
    case world::settlement::SettlementAttitudes::COLD:
        return "Cold";
    case world::settlement::SettlementAttitudes::INDIFFERENT:
        return "Indifferent";
    case world::settlement::SettlementAttitudes::OPEN:
        return "Open";
    case world::settlement::SettlementAttitudes::WELCOMING:
        return "Welcoming";
    case world::settlement::SettlementAttitudes::FRIENDLY:
        return "Friendly";
    }
}

const char *utility::GetClassName(world::group::GroupClasses groupClass)
{
    switch(groupClass)
    {
    case world::group::GroupClasses::ADVENTURER:
        return "Adventurer";
    case world::group::GroupClasses::MERCHANT:
        return "Trader";
    case world::group::GroupClasses::BANDIT:
        return "Bandit";
    case world::group::GroupClasses::TRAVELLING_COURT:
        return "Court";
    case world::group::GroupClasses::PATROL:
        return "Patrol";
    }
}