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

    if(diceRoll == 1 || difference <= -10)
    {
        return SuccessTypes::CRITICAL_FAILURE;
    }

    if(isSuccess == false)
    {
        return SuccessTypes::FAILURE;
    }

    if(diceRoll == 20 || difference >= 10)
    {
        return SuccessTypes::CRITICAL_SUCCESS;
    }

    return SuccessTypes::SUCCESS;
}