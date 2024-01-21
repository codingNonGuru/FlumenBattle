#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenBattle/World/Character/Types.h"

struct Spell;
struct CharacterAction;

namespace world::character
{
    struct CharacterClass
    {
        CharacterClasses Class;

        Word Name;

        Integer HitDice;

        Integer ArmorClass;

        Array <CharacterAction> Actions;

        struct SpellPower
        {
            bool CanCastSpells;

            AbilityTypes SpellCastingAbility;

            Array <Spell> Spells;

            SpellPower();

            SpellPower(AbilityTypes, Array <Spell>);
        };

        SpellPower SpellPower {};

        bool CanCastSpells() const {return SpellPower.CanCastSpells;}

        bool IsClass(CharacterClasses _class) const {return Class == _class;}
    };
}