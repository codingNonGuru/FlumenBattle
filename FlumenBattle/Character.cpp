#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenBattle/Character.h"
#include "FlumenBattle/CharacterInfo.h"
#include "FlumenBattle/Weapon.h"
#include "FlumenBattle/Spell.h"
#include "FlumenBattle/Condition.h"
#include "FlumenBattle/SpellCaster.h"
#include "FlumenBattle/BattleTile.h"
#include "FlumenBattle/BattleController.h"
#include "FlumenBattle/Group.h"
#include "FlumenBattle/Combatant.h"

Character::Character()
{
    weapons.Initialize(4);
    spells.Initialize(4);
    spellSlots.Initialize(8);
}

void Character::Initialize()
{
    currentHitPoints = maximumHitPoints;

    selectedAction = actions.GetStart();
    selectedWeapon = weapons.GetStart();
    selectedSpell = spells.GetStart();

    *spellSlots.Allocate() = {2};
}

bool Character::IsAlive() const
{
    return currentHitPoints > 0;
}

void Character::AddWeapon(Weapon weapon)
{
    *weapons.Allocate() = weapon;
    selectedWeapon = weapons.GetEnd() - 1;
}

void Character::AddSpell(Spell spell)
{
    *spells.Allocate() = spell;
    selectedSpell = spells.GetEnd() - 1;
}

void Character::AddProficiency(Proficiency proficiency)
{
    proficiencies.AddProficiency(proficiency);
}

Integer Character::GetActionRange() const
{
    if(IsAlive())
    {
        if(selectedAction->Type == CharacterActions::ATTACK)
        {
            return selectedWeapon->Range;
        }
        else if(selectedAction->Type == CharacterActions::CAST_SPELL)
        {
            return selectedSpell->Range;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}

Word Character::GetName()
{
    switch(type)
    {
    case CharacterClasses::FIGHTER:
        return "Fighter";
        break;
    case CharacterClasses::RANGER:
        return "Ranger";
        break;
    case CharacterClasses::CLERIC:
        return "Cleric";
        break;
    case CharacterClasses::WIZARD:
        return "Wizard";
        break;
    }
}

void Character::BoostAbility(AbilityTypes type, Integer boost)
{
    abilities.Boost(type, boost);
}

Integer Character::GetReflexSaveBonus()
{
    return proficiencies.GetSaveBonus(*this, SavingThrows::REFLEX_SAVE) + abilities.GetModifier(AbilityTypes::DEXTERITY);
}

Integer Character::GetFortitudeSaveBonus()
{
    return proficiencies.GetSaveBonus(*this, SavingThrows::FORTITUDE_SAVE) + abilities.GetModifier(AbilityTypes::CONSTITUTION);
}

Integer Character::GetWillSaveBonus()
{
    return proficiencies.GetSaveBonus(*this, SavingThrows::WILL_SAVE) + abilities.GetModifier(AbilityTypes::WISDOM);
}

bool Character::SelectAction(Index index)
{
    if(index >= actions.GetSize())
        return false;

    selectedAction = actions.Get(index);
    return true;
}

bool Character::SelectActionOption(Index index)
{
    switch(selectedAction->Type)
    {
        case CharacterActions::ATTACK:
            if(index >= weapons.GetSize())
                return false;

            selectedWeapon = weapons.Get(index);
            return true;
            break;
        case CharacterActions::CAST_SPELL:
            if(index >= spells.GetSize())
                return false;

            selectedSpell = spells.Get(index);
            return true;
            break;
        default:
            return false;
            break;
    }
}

bool Character::SelectAction(CharacterActions action)
{
    selectedAction = actions.Find(action);

    return true;
}

bool Character::SelectWeapon(Weapon *weapon)
{
    selectedWeapon = weapon;

    return true;
}

bool Character::SelectWeapon(WeaponTypes type)
{
    selectedWeapon = weapons.Find(type);
}

bool Character::SelectSpell(Spell *spell)
{
    selectedSpell = spell;

    return true;
}

bool Character::SelectSpell(SpellTypes type)
{
    selectedSpell = spells.Find(type);

    return true;
}

Index Character::GetSelectedActionIndex() const
{
    return selectedAction - actions.GetStart();
}

Index Character::GetSelectedSubactionIndex() const
{
    switch(selectedAction->Type)
    {
        case CharacterActions::ATTACK:
            return selectedWeapon - weapons.GetStart();
        case CharacterActions::CAST_SPELL:
            return selectedSpell - spells.GetStart();
        default:
            return 0;
    }
}

Character::Action* Character::GetSelectedAction() const
{
    return selectedAction;
}