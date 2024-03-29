#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenBattle/World/Character/Types.h"

struct CharacterAction;

namespace world::character
{
    struct Spell;
    
    struct CharacterClass
    {
        CharacterClasses Class;

        Word Name;

        Integer HitDice;

        Integer ArmorClass;

        struct TextureData
        {
            Word Name {};

            Position2 Offset;
        };

        TextureData TextureData;

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