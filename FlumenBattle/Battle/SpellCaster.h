#pragma once

#include "FlumenCore/Conventions.hpp"

namespace battle
{
    class Combatant;
    struct CharacterActionData;
}

namespace world::character
{
    struct Spell;
}

namespace battle
{
    class SpellCaster
    {
        static void ComputeDifficultyClass(Combatant &);

        static void RollAttack(Combatant &);

        static void RollDamage(Combatant &, const world::character::Spell &);

        static void RollHealing(Combatant &, const world::character::Spell &);

        static CharacterActionData ApplyFrostRay(Combatant &, const world::character::Spell &);

        static CharacterActionData ApplyShockingGrasp(Combatant &, const world::character::Spell &);

        static CharacterActionData ApplySacredFlame(Combatant &, const world::character::Spell &);

        static CharacterActionData ApplyFireBolt(Combatant &, const world::character::Spell &);

        static CharacterActionData ApplyCureWounds(Combatant &, const world::character::Spell &);

        static CharacterActionData ApplyHealingWord(Combatant &, const world::character::Spell &);

        static CharacterActionData ApplyBless(Combatant &, const world::character::Spell &);

    public:
        static CharacterActionData ApplyEffect(Combatant &, const world::character::Spell &);
    };
}