#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenBattle/Character.h"
#include "FlumenBattle/CharacterInfo.h"
#include "FlumenBattle/Weapon.h"
#include "FlumenBattle/Spell.h"
#include "FlumenBattle/Condition.h"
#include "FlumenBattle/SpellCaster.h"
#include "FlumenBattle/BattleTile.h"

Character::Character()
{
    weapons.Initialize(4);
    spells.Initialize(4);
    conditions.Initialize(8);
    spellSlots.Initialize(8);
}

void Character::Initialize()
{
    currentHitPoints = maximumHitPoints;
    movement = 0;
    remainingActionCount = 0;

    deathThrowSuccesCount = 0;
    deathThrowFailureCount = 0;

    isSavingAgainstDeath = false;

    selectedAction = actions.GetStart();
    selectedSpell = spells.GetStart();

    *spellSlots.Allocate() = {2};
}

bool Character::CanMove() const
{
    return IsAlive();
}

bool Character::CanAttack() const 
{
    return IsAlive() && remainingActionCount > 0;
}

bool Character::CanCastSpell() const
{
    return IsAlive() && remainingActionCount > 0;
}

bool Character::CanDodge() const
{
    return IsAlive() && remainingActionCount > 0;
}

bool Character::CanDash() const
{
    return IsAlive() && remainingActionCount > 0;
}

bool Character::IsAlive() const
{
    return currentHitPoints > 0;
}

bool Character::SufferDamage(Integer damage)
{
    Integer remainingDamage = damage - currentHitPoints;
    currentHitPoints -= damage;
    if(currentHitPoints <= 0)
    {
        currentHitPoints = 0;
        if(remainingDamage < maximumHitPoints)
        {
            isSavingAgainstDeath = true;
            deathThrowSuccesCount = 0;
            deathThrowFailureCount = 0;
        }
        else
        {
            isSavingAgainstDeath = false;
            deathThrowFailureCount = 3;
        }
    }
}

bool Character::HealDamage(Integer damage)
{
    currentHitPoints += damage;
    if(currentHitPoints > maximumHitPoints)
    {
        currentHitPoints = maximumHitPoints;
    }

    isSavingAgainstDeath = false;
    deathThrowSuccesCount = 0;
    deathThrowFailureCount = 0;
}

bool Character::HasAdvantage() const
{
    return false;
}

bool Character::HasDisadvantage() const
{
    if(GetActionRange() > 1)
    {
        auto& tiles = tile->GetNearbyTiles(1);
        for(auto otherTileIterator = tiles.GetStart(); otherTileIterator != tiles.GetEnd(); ++otherTileIterator)
        {
            auto otherTile = *otherTileIterator;
            if(tile == otherTile)
                continue;

            if(otherTile->Character != nullptr && otherTile->Character->group != group && otherTile->Character->IsAlive())
                return true;
        }
    }

    if(target->IsDodging())
    {
        return true;
    }

    return false;
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

void Character::AddCondition(Condition newCondition)
{
    bool hasFound = false;

    for(auto condition = conditions.GetStart(); condition != conditions.GetEnd(); ++condition)
    {
        if(condition->GetType() == newCondition.GetType())
        {
            *condition = newCondition;
            hasFound = true;
            break;
        }
    }
    
    if(!hasFound)
    {
        auto condition = conditions.Add();
        if(condition != nullptr)
        {
            *condition = newCondition;
        }
        else
        {
            std::cout<<"Adding condition on character has failed.\n";
        }
        
    }
}

Integer Character::RollAttackDamage() const
{
    auto sum = 0;
    for(Index i = 0; i < selectedWeapon->RollCount; ++i)
    {
        sum += utility::GetRandom(1, selectedWeapon->HitDice);
    }

    return sum;
}

bool Character::IsDodging()
{
    for(auto condition = conditions.GetStart(); condition != conditions.GetEnd(); ++condition)
    {
        if(condition->type == ConditionTypes::EVASION && condition->IsActive())
        {
            return true;
        }
    }

    return false;
}

void Character::StartTurn()
{
    speedPenalty = 0;

    for(auto condition = conditions.GetStart(); condition != conditions.GetEnd(); ++condition)
    {
        condition->Apply();
    }

    movement = GetCurrentSpeed();
    remainingActionCount = 1;
}

bool Character::CanAct(const Character & target)
{
    if(selectedAction->Type == CharacterActions::ATTACK)
    {
        return CanAttackTarget(target);
    }
    else if(selectedAction->Type == CharacterActions::CAST_SPELL)
    {
        return CanCastSpellAgainstTarget(target);
    }
    else if(selectedAction->Type == CharacterActions::DODGE)
    {
        return IsValidDodgeTarget(target);
    }
    else if(selectedAction->Type == CharacterActions::DASH)
    {
        return IsValidDashTarget(target);
    }
    else
    {
        return false;
    }
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
    if(distance > GetActionRange())
        return false;

    return true;
}

bool Character::CanCastSpellAgainstTarget(const Character & target)
{
    if((selectedSpell->IsOffensive && group == target.group) || (!selectedSpell->IsOffensive && group != target.group))
        return false;

    if(CanCastSpell() == false)
        return false;

    auto distance = tile->GetDistanceTo(*(target.tile));
    if(distance > GetActionRange())
        return false;

    if(selectedSpell->Level == 0)
        return true;
    
    auto slot = spellSlots.Get(selectedSpell->Level - 1);
    if(slot->Current == 0)
        return false;

    return true;
}

bool Character::IsValidDodgeTarget(const Character & target)
{
    if(CanDodge() == false)
        return false;

    if(&target != this)
        return false;

    return true;
}

bool Character::IsValidDashTarget(const Character & target)
{
    if(CanDash() == false)
        return false;

    if(&target != this)
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
    if(distance > movement)
        return;

    movement -= distance;

    tile->Character = nullptr;
    tile = destination;
    destination->Character = this;
}

void Character::SaveAgainstDeath()
{
    auto roll = utility::GetRandom(1, 20);
    if(roll > 10)
    {
        deathThrowSuccesCount++;
        if(roll == 20)
        {
            deathThrowSuccesCount++;
        }
    }
    else
    {
        deathThrowFailureCount++;
        if(roll == 1)
        {
            deathThrowFailureCount++;
        }
    }

    if(deathThrowSuccesCount >= 3 || deathThrowFailureCount >= 3)
    {
        isSavingAgainstDeath = false;
    }
}

CharacterActionData Character::Act(Character& target)
{
    this->target = &target;

    if(selectedAction->Type == CharacterActions::ATTACK)
    {
        return Attack();
    }
    else if(selectedAction->Type == CharacterActions::CAST_SPELL)
    {
        return CastSpell();
    }
    else if(selectedAction->Type == CharacterActions::DODGE)
    {
        return Dodge();
    }
    else if(selectedAction->Type == CharacterActions::DASH)
    {
        return Dash();
    }
    else
    {
        return CharacterActionData();
    }
}

CharacterActionData Character::Attack()
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

    if(GetActionRange() == 1)
    {
        attackAbility = &strength;
    }
    else
    {
        attackAbility = &dexterity;
    }

    attackRoll += attackAbility->Modifier + proficiencyBonus;

    Integer damage;
    if(attackRoll >= target->armorClass)
    {
        damage = RollAttackDamage() + attackAbility->Modifier;
        target->SufferDamage(damage);
    }

    remainingActionCount--;

    return CharacterActionData(selectedAction->Type, this, attackRoll, target->armorClass, damage, attackRoll > target->armorClass);
}

CharacterActionData Character::CastSpell()
{
    auto actionData = SpellCaster::ApplyEffect(*this, *selectedSpell);

    remainingActionCount--;

    return actionData;
}

CharacterActionData Character::Dodge()
{
    AddCondition(Condition(ConditionTypes::EVASION, this));

    remainingActionCount--;

    return {CharacterActions::DODGE, this, 0, 0, 0, true};
}

CharacterActionData Character::Dash()
{
    movement += GetCurrentSpeed();

    remainingActionCount--;

    return {CharacterActions::DASH, this, 0, 0, 0, true};
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

Integer Character::GetCurrentSpeed() const
{
    if(IsAlive())
    {
        return defaultSpeed - speedPenalty;
    }
    else
    {
        return 0;
    }
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
            return 1;
        }
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

Character::Action* Character::GetSelectedAction() const
{
    return selectedAction;
}