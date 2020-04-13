#pragma once

#include "FlumenCore/Conventions.hpp"

class Character;
struct Spell;
struct CharacterActionData;

class SpellCaster
{
    static void ComputeDifficultyClass(Character&);

    static void RollAttack(Character&);

    static void RollDamage(Character&, const Spell &);

    static void RollHealing(Character&, const Spell &);

    static CharacterActionData ApplyFrostRay(Character &, const Spell &);

    static CharacterActionData ApplyShockingGrasp(Character &, const Spell &);

    static CharacterActionData ApplySacredFlame(Character &, const Spell &);

    static CharacterActionData ApplyCureWounds(Character &, const Spell &);

public:
    static CharacterActionData ApplyEffect(Character &, const Spell &);
};