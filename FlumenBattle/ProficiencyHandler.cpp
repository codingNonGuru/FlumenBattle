#include "FlumenBattle/ProficiencyHandler.h"
#include "FlumenBattle/Character.h"
#include "FlumenBattle/Weapon.h"

Integer ProficiencyHandler::GetLevelModifier(Proficiency proficiency)
{
    return (int)proficiency.Level;
}

Integer ProficiencyHandler::GetPerceptionBonus(const Character &character)
{
    auto levelModifier = character.level / 6;
    auto perceptionModifier = GetLevelModifier(perception);
    auto abilityModifier = character.GetAbility(AbilityTypes::WISDOM).Modifier;
    return abilityModifier + perceptionModifier + levelModifier;
}

Integer ProficiencyHandler::GetWeaponBonus(const Character &character)
{
    auto weapon = character.GetSelectedWeapon();
    auto bonus = GetWeaponBonus(character, weapon->Class);

    return bonus;
}

Integer ProficiencyHandler::GetWeaponBonus(const Character &character, WeaponClasses weaponClass)
{
    Integer modifier = 0;
    modifier += character.level / 6;
    
    for(Proficiency *iterator = weaponClasses.GetFirst(); iterator != weaponClasses.GetLast(); ++iterator)
    {
        if(iterator->Type->Identifier == weaponClass)
        {
            modifier += GetLevelModifier(*iterator);
            break;
        }
    }

    return modifier;
}

Integer ProficiencyHandler::GetSaveBonus(const Character &character, SavingThrows savingThrow)
{
    Integer modifier = 0;
    modifier += character.level / 6;
    
    for(auto iterator = savingThrows.GetFirst(); iterator != savingThrows.GetLast(); ++iterator)
    {
        if(iterator->Type->Identifier == savingThrow)
        {
            modifier += GetLevelModifier(*iterator);
            break;
        }
    }

    return modifier;
}

Integer ProficiencyHandler::GetMagicBonus(const Character & character)
{
    Integer modifier = 0;
    modifier += character.level / 6;

    modifier += GetLevelModifier(magicalTradition);

    return modifier;
}

void ProficiencyHandler::AddProficiency(Proficiency proficiency)
{
    switch(proficiency.Type->Class)
    {
        case ProficiencyClasses::PERCEPTION:
            perception = proficiency;
            break;
        case ProficiencyClasses::MAGIC:
            magicalTradition = proficiency;
            break;
        case ProficiencyClasses::SAVE:
            *savingThrows.Add() = proficiency;
            break;
        case ProficiencyClasses::WEAPON:
            *weaponClasses.Add() = proficiency;
            break;
        case ProficiencyClasses::ARMOR:
            *armorClasses.Add() = proficiency;
            break;
        case ProficiencyClasses::SKILL:
            *skills.Add() = proficiency;
            break;
    }
}