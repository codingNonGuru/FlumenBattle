#include "FlumenCore/Utility/Utility.hpp"

#include "Character.h"
#include "FlumenBattle/CharacterInfo.h"
#include "FlumenBattle/BattleMap.h"

Character::Character()
{}

void Character::Attack(Character& target)
{
    if(this == &target)
        return;

    if(group == target.group)
        return;

    auto attackRoll = utility::GetRandom(1, 20) + primaryAbility->Modifier + proficiencyBonus;
    if(attackRoll < target.armorClass)
        return;

    target.hitPointCount -= 2;
    if(target.hitPointCount < 0)
    {
        target.hitPointCount = 0;
    }
}

void Character::Select()
{
    info->Select();
}

void Character::Deselect()
{
    info->Deselect();
}

Position2 Character::GetPosition()
{
    return tile->Position;
}