#include "FlumenBattle/World/Character/ProficiencyHandler.h"
#include "FlumenBattle/World/Character/ProficiencyFactory.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/World/Character/Weapon.h"

using namespace world::character;

void ProficiencyHandler::Initialize()
{
    savingThrows.Clear();

    weaponClasses.Clear();

    armorClasses.Clear();

    skills.Clear();
}

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
    //auto weapon = character.GetSelectedWeapon();
    //auto bonus = GetWeaponBonus(character, weapon->Class);

    //return bonus;
    return 0;
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

Integer ProficiencyHandler::GetMagicBonus(const Character & character) const
{
    Integer modifier = 0;
    modifier += character.level / 6;

    modifier += GetLevelModifier(magicalTradition);

    return modifier;
}

Integer ProficiencyHandler::GetSkillBonus(const Character &character, SkillTypes skillType)
{
    Integer modifier = 0;

    for(auto iterator = skills.GetFirst(); iterator != skills.GetLast(); ++iterator)
    {
        if(iterator->Type->Identifier == skillType)
        {
            modifier += GetLevelModifier(*iterator);
            break;
        }
    }

    const auto &skill = ProficiencyFactory::BuildSkillType(skillType);

    modifier += character.GetAbility(skill.AssociatedAbility).Modifier;

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