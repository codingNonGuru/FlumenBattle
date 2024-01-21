#include "CharacterClass.h"
#include "FlumenBattle/World/Character/Spell.h"

using namespace world::character;

CharacterClass::SpellPower::SpellPower(AbilityTypes ability, Array <Spell> spells) : 
    CanCastSpells(true), 
    SpellCastingAbility(ability), 
    Spells(spells) 
{
    
}

CharacterClass::SpellPower::SpellPower() : CanCastSpells(false) 
{}