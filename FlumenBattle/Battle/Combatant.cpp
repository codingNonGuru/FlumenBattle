#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenBattle/Battle/Combatant.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/Battle/BattleTile.h"
#include "FlumenBattle/Battle/BattleInterface.h"
#include "FlumenBattle/World/Character/Condition.h"
#include "FlumenBattle/World/Character/Spell.h"
#include "FlumenBattle/Battle/SpellCaster.h"
#include "FlumenBattle/World/Character/Modifier.h"
#include "FlumenBattle/World/Character/Weapon.h"
#include "FlumenBattle/Battle/CombatGroup.h"
#include "FlumenBattle/Battle/CharacterInfo.h"
#include "FlumenBattle/World/Character/CharacterClass.h"
#include "FlumenBattle/World/Character/CharacterAction.h"
#include "FlumenBattle/World/Character/Types.h"
#include "FlumenBattle/Utility/Utility.h"
#include "FlumenBattle/Config.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/Battle/BattleScene.h"
#include "FlumenBattle/World/Group/Encounter.h"

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

    rotation = 0.0f;

    world::character::ConditionAllocator::Initialize(conditions, 16);

    world::character::ModifierAllocator::Initialize(modifiers, 16);
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

Position2 Combatant::GetPosition() const
{
    return position;
}

float Combatant::GetRotation() const
{
    return rotation;
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
        auto modifier = character->GetModifier(world::character::Modifiers::MOVE_SPEED);

        auto speed = character->defaultSpeed + modifier;
        if(speed < 0)
            speed = 0;

        return speed;
    }
    else
    {
        return 0;
    }
}

Integer Combatant::GetArmorClass() const
{
    return character->GetArmorClass() + modifiers.GetAmount(world::character::Modifiers::BONUS_ARMOR_CLASS);
}

Integer Combatant::GetAttackRating() const
{
    return character->GetAttackRating() + modifiers.GetAmount(world::character::Modifiers::ATTACK_RATING_BONUS);
}

bool Combatant::IsWithinActionRange(BattleTile *tile)
{
    auto distance = this->tile->GetDistanceTo(*tile);

    auto actionRange = character->GetActionRange();

    return distance <= actionRange;
}

void Combatant::StartTurn()
{
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
    auto initiative = utility::RollD20Dice();

    initiative += character->GetInitiativeModifier();

    return initiative;
}

bool Combatant::CanMove(int distance) const
{
    return character->IsAlive() && movement > 0 && distance <= movement;
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
        return CanCastSpell(true);
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

bool Combatant::CanAct(BattleTile *possibleTarget)
{
    targetTile = possibleTarget;

    if(character->selectedAction->Type == world::character::CharacterActions::ATTACK)
    {
        return false;
    }
    else if(character->selectedAction->Type == world::character::CharacterActions::CAST_SPELL)
    {
        return CanCastSpell(false);
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

bool Combatant::CanCastSpell(bool isCombatantTargeted) const
{
    if(character->IsAlive() == false)
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

    if(isCombatantTargeted == true)
    {
        if(target == nullptr)
            return false;

        if((character->selectedSpell->IsOffensive && GetGroup() == target->GetGroup()) || (!character->selectedSpell->IsOffensive && GetGroup() != target->GetGroup()))
            return false;
    }
    else
    {
        if(targetTile == nullptr)
            return false;
    }

    auto distance = tile->GetDistanceTo(isCombatantTargeted ? *(target->tile) : *targetTile);
    if(distance > character->GetActionRange())
        return false;

    if(character->selectedSpell->Level == 0)
        return true;
    
    auto slot = character->spellUseCount;//character->spellSlots.Get(character->selectedSpell->Level - 1);
    if(slot == 0)
        return false;

    if(isCombatantTargeted == false && character->selectedSpell->CanTargetTiles == false)
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

bool Combatant::Move(BattleTile *destination, int distance)
{
    if(destination->Combatant != nullptr)
        return false;

    if(CanMove(distance) == false)
        return false;

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

    targetTile = nullptr;

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

CharacterActionData Combatant::Act(BattleTile *finalTarget)
{
    targetTile = finalTarget;

    target = nullptr;

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
    auto targetArmor = target->GetArmorClass();

    auto distance = GetDistanceTo(target);
    if(distance == 1)
    {
        if(target->IsFlanked() == true)
        {
            static const auto FLANKING_ARMOR_PENALTY = engine::ConfigManager::Get()->GetValue(game::ConfigValues::FLANKING_ARMOR_PENALTY).Integer;

            targetArmor -= FLANKING_ARMOR_PENALTY;
        }
    }

    auto attackBonus = GetAttackRating();

    auto attackRoll = utility::RollD20Dice(targetArmor, attackBonus);

    auto damage = 0;

    bool hasHit = attackRoll.IsAnySuccess();
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

    return CharacterActionData(character->selectedAction->Type, this, attackRoll.Roll + attackRoll.Modifier, targetArmor, damage, hasHit);
}

CharacterActionData Combatant::CastSpell()
{
    auto actionData = SpellCaster::ApplyEffect(this, targetTile, *character->selectedSpell);

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
    //AddCondition(world::character::Conditions::EVASION, character);

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


/*void Combatant::AddCondition(world::character::Conditions newCondition, world::character::Character *character)
{
    character->AddCondition({newCondition});
}*/

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

bool Combatant::IsFlanked()
{
    auto nearbyEnemyCount = 0;

    auto &nearbyTiles = tile->GetNearbyTiles(1);
    for(auto nearbyTile : nearbyTiles)
    {
        auto other = nearbyTile->Combatant;
        if(other != nullptr && other->IsAlive() == true && other->GetCharacter()->GetGroup() != character->GetGroup())
        {
            nearbyEnemyCount++;
        }
    }

    if(nearbyEnemyCount > 1)
        return true;
    else 
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

void Combatant::AddModifier(world::character::Modifier modifier)
{
    modifiers.AddModifier(modifier);
}

void Combatant::AddCondition(world::character::ConditionData data)
{
    conditions.AddCondition(data);

    RefreshModifiers();
}

void Combatant::ApplyPermanentConditions()
{
    const auto settlement = character->GetGroup()->GetCurrentSettlement();
    if(settlement != nullptr)
    {
        const auto battle = character->GetGroup()->GetEncounter();
        if(battle->GetDefender() == character->GetGroup() && battle->IsSiege() == true)
        {
            auto modifier = settlement->GetModifier(world::settlement::Modifiers::DEFENDER_GROUP_BONUS_AC);
            if(modifier != 0)
            {
                AddCondition({world::character::Conditions::WALL_PROTECTION, 1, modifier});
            }

            modifier = settlement->GetModifier(world::settlement::Modifiers::DEFENDER_GROUP_BONUS_INITIATIVE);
            if(modifier != 0)
            {
                AddCondition({world::character::Conditions::ALERT, 1, modifier});
            }
        }
    }
}

void Combatant::UpdateConditions(int turnIndex) 
{
    conditions.UpdateBattle(turnIndex);
}

void Combatant::RefreshModifiers()
{
    modifiers.ClearModifiers();

    conditions.ApplyModifiers(*this);
}