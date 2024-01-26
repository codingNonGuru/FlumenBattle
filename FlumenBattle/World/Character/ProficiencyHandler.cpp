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
        case CheckClasses::MAGIC:
            magicalTradition = proficiency;
            break;
        case CheckClasses::SAVE:
            *savingThrows.Add() = proficiency;
            break;
        case CheckClasses::WEAPON:
            *weaponClasses.Add() = proficiency;
            break;
        case CheckClasses::ARMOR:
            *armorClasses.Add() = proficiency;
            break;
        case CheckClasses::SKILL:
            *skills.Add() = proficiency;
            break;
    }
}