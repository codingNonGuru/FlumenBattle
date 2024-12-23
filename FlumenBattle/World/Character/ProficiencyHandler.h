#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenBattle/World/Character/Proficiency.h"

namespace world::character
{
    class Character;

    class ProficiencyHandler
    {
        Proficiency magicalTradition;

        container::SmartBlock <Proficiency, 3> savingThrows;

        container::SmartBlock <Proficiency, 4> weaponClasses;

        container::SmartBlock <Proficiency, 4> armorClasses;

        container::SmartBlock <Proficiency, 8> skills;

        static Integer GetLevelModifier(Proficiency);

    public:
        void Initialize();

        Integer GetSaveBonus(const Character &, SavingThrows);

        Integer GetWeaponBonus(const Character &, WeaponClasses);

        Integer GetWeaponBonus(const Character &);

        Integer GetMagicBonus(const Character &) const;

        Integer GetSkillBonus(const Character &, SkillTypes);

        bool IsTrainedInSkill(const Character &, SkillTypes) const;

        void AddProficiency(Proficiency proficiency);
    };
}