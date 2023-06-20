#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenBattle/Utility/Utility.h"

utility::Result utility::RollD4Dice()
{
    return utility::GetRandom(1, 4);
}

utility::Result utility::RollD20Dice()
{
    return utility::GetRandom(1, 20);
}

utility::Result utility::RollD100Dice()
{
    return utility::GetRandom(1, 100);
}

utility::Success utility::RollD20Dice(DifficultyClass difficultyClass, Bonus bonus = 0)
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

    return success;
}