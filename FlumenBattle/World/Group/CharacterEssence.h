#pragma once

#include "FlumenBattle/World/Character/Types.h"

namespace world::group
{
    struct CharacterEssence
    {
        bool isFunctioning;

        bool isAlive;

        character::CharacterClasses characterClass;

        int experience;

        int level;

        int health;

        void Initialize(character::CharacterClasses);

        bool SufferDamage(int);

        int GetMaximumHitpoints() const;

        void ResetHitpoints();

        void GainExperience(int);
    };
}