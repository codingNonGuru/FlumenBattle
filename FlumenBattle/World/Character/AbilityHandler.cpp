#include "AbilityHandler.h"
#include "FlumenBattle/World/Character/CharacterClass.h"
#include "FlumenBattle/World/Character/Character.h"

using namespace world::character;

const Ability *AbilityHandler::GetSpellCastingAbility(const Character &character) const
{
    auto ability = character.type->SpellPower.SpellCastingAbility;
    return GetAbility(ability);
}

const int &AbilityHandler::GetSpellCastingAbilityModifier(const Character &character) const
{
    return GetSpellCastingAbility(character)->Modifier;
}