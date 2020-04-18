#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenBattle/Proficiency.h"

class Character;

class ProficiencyHandler
{
    Proficiency perception;

    Proficiency magicalTradition;

    container::SmartBlock <Proficiency, 3> savingThrows;

    container::SmartBlock <Proficiency, 4> weaponClasses;

    container::SmartBlock <Proficiency, 4> armorClasses;

    container::SmartBlock <Proficiency, 8> skills;

    static Integer GetLevelModifier(Proficiency);

public:
    Integer GetPerceptionBonus(const Character &);

    Integer GetWeaponBonus(const Character &, WeaponClasses);

    Integer GetWeaponBonus(const Character &);

    void AddProficiency(Proficiency proficiency);
};