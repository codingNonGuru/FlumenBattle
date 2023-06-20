#pragma once

#include "FlumenBattle/Utility/Types.h"

namespace utility
{
    typedef int Result;

    Result RollD4Dice();

    Result RollD20Dice();

    typedef int Bonus;

    typedef int DifficultyClass;

    class Success
    {
        SuccessTypes Type;

        Result Roll;

        Result ModifiedRoll;

    public:
        Success() {}

        Success(const SuccessTypes &type) : Type(type) {}

        bool IsAnySuccess() const {return Type == SuccessTypes::SUCCESS || Type == SuccessTypes::CRITICAL_SUCCESS;}

        bool IsNormalSuccess() const {return Type == SuccessTypes::SUCCESS;}

        bool IsCriticalSuccess() const {return Type == SuccessTypes::CRITICAL_SUCCESS;}

        //bool operator ==(const Success &other) {return other.Type == Type;}

        bool operator ==(const SuccessTypes &other) const {return other == Type;}

        Result GetRollValue() const {return Roll;}

        operator SuccessTypes() {return Type;}
    };

    Success RollD20Dice(DifficultyClass, Bonus = 0);

    Result RollD100Dice();
}