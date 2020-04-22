#pragma once

#include "FlumenCore/Conventions.hpp"

class Combatant;
struct Spell;
struct CharacterActionData;

class SpellCaster
{
    static void ComputeDifficultyClass(Combatant &);

    static void RollAttack(Combatant &);

    static void RollDamage(Combatant &, const Spell &);

    static void RollHealing(Combatant &, const Spell &);

    static CharacterActionData ApplyFrostRay(Combatant &, const Spell &);

    static CharacterActionData ApplyShockingGrasp(Combatant &, const Spell &);

    static CharacterActionData ApplySacredFlame(Combatant &, const Spell &);

    static CharacterActionData ApplyFireBolt(Combatant &, const Spell &);

    static CharacterActionData ApplyCureWounds(Combatant &, const Spell &);

    static CharacterActionData ApplyHealingWord(Combatant &, const Spell &);

public:
    static CharacterActionData ApplyEffect(Combatant &, const Spell &);
};