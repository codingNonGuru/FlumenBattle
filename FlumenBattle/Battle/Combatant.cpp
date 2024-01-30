#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenBattle/Battle/Combatant.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/Battle/BattleTile.h"
#include "FlumenBattle/Battle/BattleInterface.h"
#include "FlumenBattle/World/Character/Condition.h"
#include "FlumenBattle/World/Character/Spell.h"
#include "FlumenBattle/Battle/SpellCaster.h"
#include "FlumenBattle/World/Character/Condition.h"
#include "FlumenBattle/World/Character/Weapon.h"
#include "FlumenBattle/Battle/CombatGroup.h"
#include "FlumenBattle/Battle/CharacterInfo.h"
#include "FlumenBattle/World/Character/CharacterClass.h"
#include "FlumenBattle/World/Character/CharacterAction.h"
#include "FlumenBattle/World/Character/Types.h"
#include "FlumenBattle/Utility/Utility.h"

using namespace battle;

Combatant::Combatant()
{
    //conditions.Initialize(16);
}

void Combatant::Initialize(CombatGroup *_group, world::character::Character *_character, BattleTile *_tile)
{
    group = _group;
    character = _character;

    tile = nullptr;
    SetTile(_tile);

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
    return position;
}

Integer Combatant::GetDistanceTo(Combatant *other) const
{
    return tile->GetDistanceTo(*other->tile);
}

const Integer &Combatant::GetMovement() const
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

bool Combatant::IsWithinActionRange(BattleTile *tile)
{
    auto distance = this->tile->GetDistanceTo(*tile);

    auto actionRange = character->GetActionRange();

    return distance <= actionRange;
}

void Combatant::StartTurn()
{
    speedPenalty = 0;

    /*conditions.Do([](auto &condition) -> bool
    {
        condition.Apply();

        return true;
    });*/

    movement = GetCurrentSpeed();

    remainingActionCount = 1;
    bonusActionCount = 1;
}

Integer Combatant::RollInitiative() const
{
    auto initiative = utility::GetRandom(1, 20);
    initiative += character->abilities.GetModifier(world::character::AbilityTypes::DEXTERITY);
    return initiative;
}

bool Combatant::CanMove(BattleTile *destination) const
{
    return character->IsAlive() && movement > 0 && destination->GetDistanceTo(*tile) <= movement;
}

bool Combatant::CanTarget() const
{
    if(!character->IsAlive())
        return false;

    if(character->selectedAction == nullptr)
        return false;

    if(character->selectedAction->Type == world::character::CharacterActions::ATTACK && remainingActionCount == 0)
        return false;

    if(character->selectedAction->Type == world::character::CharacterActions::CAST_SPELL)
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

    if(character->selectedAction->Type == world::character::CharacterActions::DODGE)
        return false;

    if(character->selectedAction->Type == world::character::CharacterActions::DASH)
        return false;
    
    return true;
}

bool Combatant::CanAct(Combatant *possibleTarget)
{
    target = possibleTarget;

    if(character->selectedAction->Type == world::character::CharacterActions::ATTACK)
    {
        return CanStrike();
    }
    else if(character->selectedAction->Type == world::character::CharacterActions::CAST_SPELL)
    {
        return CanCastSpell();
    }
    else if(character->selectedAction->Type == world::character::CharacterActions::DODGE)
    {
        return CanDodge();
    }
    else if(character->selectedAction->Type == world::character::CharacterActions::DASH)
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
    
    auto slot = character->spellUseCount;//character->spellSlots.Get(character->selectedSpell->Level - 1);
    if(slot == 0)
        return false;

    return true;
}

bool Combatant::HasSlot(SpellTypes spellType)
{
    auto &spells = character->GetSpells();
    if(auto spell = spells.Find(spellType))
    {
        auto slot = character->spellUseCount;//character->spellSlots.Get(spell->Level - 1);
        return slot > 0;
    }
    else
    {
        return false;
    }
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

    if(CanMove(destination) == false)
        return false;

    auto distance = tile->GetDistanceTo(*destination);
    if(distance > movement)
        return false;

    movement -= distance;

    SetTile(destination);

    return true;
}

void Combatant::SetTile(BattleTile *newTile)
{
    if(tile != nullptr)
    {
        tile->Combatant = nullptr;
    }

    tile = newTile;

    tile->Combatant = this;

    position = tile->Position;
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

    if(character->selectedAction->Type == world::character::CharacterActions::ATTACK)
    {
        return Strike();
    }
    else if(character->selectedAction->Type == world::character::CharacterActions::CAST_SPELL)
    {
        return CastSpell();
    }
    else if(character->selectedAction->Type == world::character::CharacterActions::DODGE)
    {
        return Dodge();
    }
    else if(character->selectedAction->Type == world::character::CharacterActions::DASH)
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
    auto targetArmor = target->GetCharacter()->GetArmorClass();

    auto attackBonus = GetCharacter()->GetAttackRating();

    auto attackRoll = utility::RollD20Dice(targetArmor, attackBonus);

    auto damage = 0;

    bool hasHit = true;//attackRoll.IsAnySuccess();
    if(hasHit)
    {
        auto damageRollInput = GetCharacter()->GetDamage();
        
        damage = utility::RollDice(damageRollInput);

        if(attackRoll.IsCriticalSuccess() == true)
        {
            damage += utility::RollDice(damageRollInput);
        }

        target->SufferDamage(damage);
    }

    remainingActionCount--;

    return CharacterActionData(character->selectedAction->Type, this, attackBonus, targetArmor, damage, hasHit);
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
    AddCondition(world::character::Conditions::EVASION, character);

    remainingActionCount--;

    return {world::character::CharacterActions::DODGE, this, 0, 0, 0, true};
}

CharacterActionData Combatant::Dash()
{
    movement += GetCurrentSpeed();

    remainingActionCount--;

    return {world::character::CharacterActions::DASH, this, 0, 0, 0, true};
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

    BattleInterface::Get()->GetDamageCounter(damage, this);
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


void Combatant::AddCondition(world::character::Conditions newCondition, world::character::Character *character)
{
    bool hasFound = false;

    /*conditions.Do([&](auto &condition) -> bool 
    {
        if(condition.GetType() == newCondition.GetType() && condition.IsActive())
        {
            condition = newCondition;
            hasFound = true;
            BREAK
        }
    });
    
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
        
    }*/
}

void Combatant::Select()
{
    info->Select();
}

void Combatant::Deselect()
{
    info->Deselect();
}

bool Combatant::IsDodging()
{
    /*bool isDodging = false;
    conditions.Do([&](auto &condition)
    {
        if(condition.type == ConditionTypes::EVASION && condition.IsActive())
        {
            isDodging = true;
            BREAK
        }
    });

    return isDodging;*/
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