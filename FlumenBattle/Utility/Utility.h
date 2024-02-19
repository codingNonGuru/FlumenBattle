#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenCore/Singleton.h"

#include "FlumenBattle/Utility/Types.h"

namespace world::settlement
{
    enum class SettlementAttitudes;
}

<<<<<<< HEAD
=======
namespace world::group
{
    enum class GroupClasses;
}

namespace world::character
{
    enum class CharacterClasses;
}

>>>>>>> testBranch
namespace utility
{
    typedef int Result;

    Result RollD4Dice();

    Result RollD6Dice();

    Result RollD8Dice();

    Result RollD10Dice();

    Result RollD12Dice();

    Result RollD20Dice();

    typedef int Bonus;

    typedef int DifficultyClass;

    struct RollMaterial
    {
        RollDies Die;

        int DieCount;

        int Bonus;
    };

    Result RollDice(RollMaterial);

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

        bool IsRegularFailure() const {return Type == SuccessTypes::FAILURE;}

        bool IsCriticalSuccess() const {return Type == SuccessTypes::CRITICAL_SUCCESS;}

        bool IsCriticalFailure() const {return Type == SuccessTypes::CRITICAL_FAILURE;}

        //bool operator ==(const Success &other) {return other.Type == Type;}

        bool operator ==(const SuccessTypes &other) const {return other == Type;}

        operator SuccessTypes() {return Type;}
    };

    Success RollD20Dice(DifficultyClass, Bonus = 0);

    Result RollD100Dice();

    const char *GetAttitudeName(world::settlement::SettlementAttitudes);
<<<<<<< HEAD
=======

    const char *GetClassName(world::group::GroupClasses);

    const char *GetClassName(world::character::CharacterClasses);
>>>>>>> testBranch
}