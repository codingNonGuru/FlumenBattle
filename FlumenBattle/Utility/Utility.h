#pragma once

#include "FlumenBattle/Utility/Types.h"

namespace utility
{
    typedef int Result;

    Result RollD4Dice();

    Result RollD20Dice();

    typedef int Bonus;

    typedef int DifficultyClass;

    struct Success
    {
        SuccessTypes Type;

        Result Roll;

        Bonus Modifier;

        DifficultyClass DC;

    public:
        Success() {}

        Success(const SuccessTypes &type) : Type(type) {}

        Success(SuccessTypes type, Result roll, Bonus modifier, DifficultyClass dC) : 
        Type(type), Roll(roll), Modifier(modifier), DC(dC) {}

        bool IsAnyFailure() const {return Type == SuccessTypes::FAILURE || Type == SuccessTypes::CRITICAL_FAILURE;}

        bool IsAnySuccess() const {return Type == SuccessTypes::SUCCESS || Type == SuccessTypes::CRITICAL_SUCCESS;}

        bool IsNormalSuccess() const {return Type == SuccessTypes::SUCCESS;}

        bool IsCriticalSuccess() const {return Type == SuccessTypes::CRITICAL_SUCCESS;}

        //bool operator ==(const Success &other) {return other.Type == Type;}

        bool operator ==(const SuccessTypes &other) const {return other == Type;}

        operator SuccessTypes() {return Type;}
    };

    Success RollD20Dice(DifficultyClass, Bonus = 0);

    Result RollD100Dice();
}