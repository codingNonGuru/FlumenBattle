#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenBattle/Combatant.h"
#include "FlumenBattle/Character.h"
#include "FlumenBattle/BattleTile.h"
#include "FlumenBattle/Condition.h"
#include "FlumenBattle/Spell.h"
#include "FlumenBattle/SpellCaster.h"
#include "FlumenBattle/CharacterInfo.h"
#include "FlumenBattle/Weapon.h"
#include "FlumenBattle/CombatGroup.h"
#include "FlumenBattle/CharacterClass.h"

Combatant::Combatant()
{
    conditions.Initialize(16);
}

void Combatant::Initialize(CombatGroup *_group, Character *_character, BattleTile *_tile)
{
    group = _group;
    character = _character;
    tile = _tile;

    movement = 0;

    remainingActionCount = 0;
    bonusActionCount = 0;

    deathThrowSuccesCount = 0;
    deathThrowFailureCount = 0;

    isSavingAgainstDeath = false;

    armorClass = character->type->ArmorClass;
}

bool Combatant::HasAdvantage() const
{
    return false;
}

bool Combatant::HasDisadvantage() const
{
    if(character->GetActionRange() > 1)
    {
        auto& tiles = tile->GetNearbyTiles(1);
        for(auto otherTileIterator = tiles.GetStart(); otherTileIterator != tiles.GetEnd(); ++otherTileIterator)
        {
            auto otherTile = *otherTileIterator;
            if(tile == otherTile)
                continue;

            if(otherTile->Combatant != nullptr && otherTile->Combatant->GetGroup() != GetGroup() && otherTile->Combatant->IsAlive())
                return true;
        }
    }

    if(target->IsDodging())
    {
        return true;
    }

    return false;
}

Position2 Combatant::GetPosition()
{
    return tile->Position;
}

Integer Combatant::GetDistanceTo(Combatant *other) const
{
    return tile->GetDistanceTo(*other->tile);
}

Integer Combatant::GetMovement() const
{
    return movement;
}

Integer Combatant::GetCurrentSpeed() const
{
    if(IsAlive())
    {
        return character->defaultSpeed - speedPenalty;
    }
    else
    {
        return 0;
    }
}

void Combatant::StartTurn()
{
    speedPenalty = 0;

    for(auto condition = conditions.GetStart(); condition != conditions.GetEnd(); ++condition)
    {
        condition->Apply();
    }

    movement = GetCurrentSpeed();

    remainingActionCount = 1;
    bonusActionCount = 1;
}

Integer Combatant::RollInitiative() const
{
    auto initiative = utility::GetRandom(1, 20);
    initiative += character->abilities.GetModifier(AbilityTypes::DEXTERITY);
    return initiative;
}

bool Combatant::CanMove() const
{
    return character->IsAlive();
}

bool Combatant::CanTarget() const
{
    if(!character->IsAlive())
        return false;

    if(character->selectedAction->Type == CharacterActions::ATTACK && remainingActionCount == 0)
        return false;

    if(character->selectedAction->Type == CharacterActions::CAST_SPELL)
    {
        if(character->selectedSpell->IsBonusAction)
        {
            if(bonusActionCount == 0)
                return false;
        }
        else
        {
            if(remainingActionCount == 0)
                return false;
        }
    }

    if(character->selectedAction->Type == CharacterActions::DODGE)
        return false;

    if(character->selectedAction->Type == CharacterActions::DASH)
        return false;
    
    return true;
}

bool Combatant::CanAct(Combatant *possibleTarget)
{
    target = possibleTarget;

    if(character->selectedAction->Type == CharacterActions::ATTACK)
    {
        return CanStrike();
    }
    else if(character->selectedAction->Type == CharacterActions::CAST_SPELL)
    {
        return CanCastSpell();
    }
    else if(character->selectedAction->Type == CharacterActions::DODGE)
    {
        return CanDodge();
    }
    else if(character->selectedAction->Type == CharacterActions::DASH)
    {
        return CanDash();
    }
    else
    {
        return false;
    }
}

bool Combatant::CanStrike() const
{
    if(target == nullptr)
        return false;

    if(this == target)
        return false;

    if(GetGroup() == target->GetGroup())
        return false;

    if(!character->IsAlive() || remainingActionCount == 0)
        return false;

    auto distance = tile->GetDistanceTo(*(target->tile));
    if(distance > character->GetActionRange())
        return false;

    return true;
}

bool Combatant::CanCastSpell() const
{
    if(target == nullptr)
        return false;

    if((character->selectedSpell->IsOffensive && GetGroup() == target->GetGroup()) || (!character->selectedSpell->IsOffensive && GetGroup() != target->GetGroup()))
        return false;

    if(!character->IsAlive())
        return false;

    if(character->selectedSpell->IsBonusAction)
    {
        if(bonusActionCount == 0)
            return false;
    }
    else
    {
        if(remainingActionCount == 0)
            return false;
    }

    auto distance = tile->GetDistanceTo(*(target->tile));
    if(distance > character->GetActionRange())
        return false;

    if(character->selectedSpell->Level == 0)
        return true;
    
    auto slot = character->spellSlots.Get(character->selectedSpell->Level - 1);
    if(slot->Current == 0)
        return false;

    return true;
}

bool Combatant::CanDodge() const
{
    return character->IsAlive() && remainingActionCount > 0;
}

bool Combatant::CanDash() const
{
    return character->IsAlive() && remainingActionCount > 0;
}

bool Combatant::Move(BattleTile *destination)
{
    if(destination->Combatant != nullptr)
        return false;

    if(CanMove() == false)
        return false;

    auto distance = tile->GetDistanceTo(*destination);
    if(distance > movement)
        return false;

    movement -= distance;

    tile->Combatant = nullptr;
    tile = destination;
    destination->Combatant = this;

    return true;
}

void Combatant::SaveAgainstDeath()
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

CharacterActionData Combatant::Act(Combatant *finalTarget)
{
    target = finalTarget;

    if(character->selectedAction->Type == CharacterActions::ATTACK)
    {
        return Strike();
    }
    else if(character->selectedAction->Type == CharacterActions::CAST_SPELL)
    {
        return CastSpell();
    }
    else if(character->selectedAction->Type == CharacterActions::DODGE)
    {
        return Dodge();
    }
    else if(character->selectedAction->Type == CharacterActions::DASH)
    {
        return Dash();
    }
    else
    {
        return CharacterActionData();
    }
}

CharacterActionData Combatant::Strike()
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

    bool isCriticalHit = attackRoll == 20;

    if(character->GetActionRange() == 1 || character->selectedWeapon->IsThrown)
    {
        character->abilities.SetAttackAbility(AbilityTypes::STRENGTH);
    }
    else
    {
        character->abilities.SetAttackAbility(AbilityTypes::DEXTERITY);
    }

    auto attackModifier = attackRoll;

    auto attackAbility = character->abilities.GetAttackAbility();
    attackModifier += attackAbility->Modifier;

    attackModifier += character->proficiencies.GetWeaponBonus(*character);

    Integer damage;
    bool hasHit = attackModifier >= target->armorClass || isCriticalHit;
    if(hasHit)
    {
        damage = RollAttackDamage(isCriticalHit) + attackAbility->Modifier;

        target->SufferDamage(damage);
    }

    remainingActionCount--;

    return CharacterActionData(character->selectedAction->Type, this, attackModifier, target->armorClass, damage, hasHit);
}

CharacterActionData Combatant::CastSpell()
{
    auto actionData = SpellCaster::ApplyEffect(*this, *character->selectedSpell);

    if(character->selectedSpell->IsBonusAction)
    {
        bonusActionCount--;
    }
    else
    {
        remainingActionCount--;
    }

    return actionData;
}

CharacterActionData Combatant::Dodge()
{
    AddCondition(Condition(ConditionTypes::EVASION, this));

    remainingActionCount--;

    return {CharacterActions::DODGE, this, 0, 0, 0, true};
}

CharacterActionData Combatant::Dash()
{
    movement += GetCurrentSpeed();

    remainingActionCount--;

    return {CharacterActions::DASH, this, 0, 0, 0, true};
}

bool Combatant::SufferDamage(Integer damage)
{
    Integer remainingDamage = damage - character->currentHitPoints;
    character->currentHitPoints -= damage;
    if(character->currentHitPoints <= 0)
    {
        character->currentHitPoints = 0;

        if(remainingDamage < character->maximumHitPoints)
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

bool Combatant::HealDamage(Integer damage)
{
    character->currentHitPoints += damage;
    if(character->currentHitPoints > character->maximumHitPoints)
    {
        character->currentHitPoints = character->maximumHitPoints;
    }

    isSavingAgainstDeath = false;
    deathThrowSuccesCount = 0;
    deathThrowFailureCount = 0;
}


void Combatant::AddCondition(Condition newCondition)
{
    bool hasFound = false;

    for(auto condition = conditions.GetStart(); condition != conditions.GetEnd(); ++condition)
    {
        if(condition->GetType() == newCondition.GetType() && condition->IsActive())
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

void Combatant::Select()
{
    info->Select();
}

void Combatant::Deselect()
{
    info->Deselect();
}

Integer Combatant::RollAttackDamage(bool isCritical) const
{
    auto weapon = character->selectedWeapon;

    auto sum = 0;
    for(Index i = 0; i < weapon->RollCount; ++i)
    {
        sum += utility::GetRandom(1, weapon->HitDice);
    }

    if(isCritical)
    {
        sum += weapon->RollCount * weapon->HitDice;
    }

    return sum;
}

bool Combatant::IsDodging()
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

CombatGroup * Combatant::GetGroup() const
{
    return group;
}

bool Combatant::IsAlive() const
{
    return character->IsAlive();
}

Float Combatant::GetHealth() const 
{
    return character->GetHealth();
}