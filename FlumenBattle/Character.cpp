#include "FlumenCore/Utility/Utility.hpp"

#include "Character.h"
#include "FlumenBattle/CharacterInfo.h"
#include "FlumenBattle/BattleMap.h"

Character::Character()
{}

void Character::Initialize()
{
    currentHitPoints = maximumHitPoints;
    movement = 0;
    actionCount = 0;
}

bool Character::CanMove() const
{
    return IsAlive();
}

bool Character::CanAttack() const 
{
    return IsAlive() && actionCount > 0;
}

bool Character::IsAlive() const
{
    return currentHitPoints > 0;
}

bool Character::SufferDamage(Integer damage)
{
    currentHitPoints -= damage;
    if(currentHitPoints < 0)
    {
        currentHitPoints = 0;
    }
}

bool Character::HasAdvantage() const
{
    return false;
}

bool Character::HasDisadvantage() const
{
    if(defaultRange > 1)
    {
        auto& tiles = tile->GetNearbyTiles(1);
        for(auto otherTileIterator = tiles.GetStart(); otherTileIterator != tiles.GetEnd(); ++otherTileIterator)
        {
            auto otherTile = *otherTileIterator;
            if(tile == otherTile)
                continue;

            if(otherTile->Character != nullptr && otherTile->Character->group != group)
                return true;
        }
    }

    return false;
}

void Character::StartTurn()
{
    movement = GetSpeed();
    actionCount = 1;
}

bool Character::CanAttackTarget(const Character & target)
{
    if(this == &target)
        return false;

    if(group == target.group)
        return false;

    if(CanAttack() == false)
        return false;

    auto distance = tile->GetDistanceTo(*(target.tile));
    if(distance > defaultRange)
        return false;

    return true;
}

void Character::Move(BattleTile* destination)
{
    if(destination->Character != nullptr)
        return;

    if(CanMove() == false)
        return;

    auto distance = tile->GetDistanceTo(*destination);
    if(distance > GetSpeed())
        return;

    movement -= distance;

    tile->Character = nullptr;
    tile = destination;
    destination->Character = this;
}

CharacterActionData Character::Attack(Character& target)
{
    bool hasDisadvantage = HasDisadvantage();

    auto attackRoll = utility::GetRandom(1, 20);
    if(hasDisadvantage)
    {
        auto newRoll = utility::GetRandom(1, 20);
        if(newRoll < attackRoll)
        {
            attackRoll = newRoll;
        }
    }
    attackRoll += primaryAbility->Modifier + proficiencyBonus;

    Integer damage;
    if(attackRoll >= target.armorClass)
    {
        damage = utility::GetRandom(1, 6) + primaryAbility->Modifier;
        target.SufferDamage(damage);
    }

    actionCount--;

    return CharacterActionData(this, &target, attackRoll, target.armorClass, damage);
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

Integer Character::GetSpeed() const
{
    if(IsAlive())
    {
        return defaultSpeed;
    }
    else
    {
        return 0;
    }
}

Integer Character::GetMovement() const
{
    return movement;
}

Integer Character::RollInitiative() const
{
    auto initiative = utility::GetRandom(1, 20);
    initiative += dexterity.Modifier;
    return initiative;
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