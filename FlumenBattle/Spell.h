#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenBattle/Types.hpp"

struct Spell
{
    SpellTypes Type;

    Integer HitDice;

    Integer RollCount;

    Integer Range;

    Word Name;

    bool IsSelfTargeted;

    //Phrase Description;
};

class SpellFactory
{
public:
    static Spell BuildRayOfFrost()
    {
        return {SpellTypes::RAY_OF_FROST, 8, 1, 10, "Ray of Frost", false};
    }

    static Spell BuildShockingGrasp()
    {
        return {SpellTypes::SHOCKING_GRASP, 8, 1, 1, "Shocking Grasp", false};
    }

    static Spell BuildMagicMissile()
    {
        return {SpellTypes::MAGIC_MISSILE, 4, 1, 10, "Magic Missile", false};
    }

    static Spell BuildCureWounds()
    {
        return {SpellTypes::CURE_WOUNDS, 6, 1, 5, "Cure Wounds", true};
    }
};