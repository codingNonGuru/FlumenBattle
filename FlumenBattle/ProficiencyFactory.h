#pragma once

#include "FlumenBattle/Proficiency.h"

class Proficiency;

class ProficiencyFactory
{
public:
    static Proficiency BuildReflexSaveProficiency(ProficiencyLevels level)
    {
        static ProficiencyType type = {ProficiencyClasses::SAVE, "Reflex save", SavingThrows::REFLEX_SAVE};
        return {level, &type};
    }
};