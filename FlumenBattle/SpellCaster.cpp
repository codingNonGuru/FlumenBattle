#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenBattle/SpellCaster.h"
#include "FlumenBattle/Spell.h"
#include "FlumenBattle/Combatant.h"
#include "FlumenBattle/Character.h"
#include "FlumenBattle/CharacterActionData.h"
#include "FlumenBattle/Condition.h"

struct SpellResult 
{
    Integer AttackRoll;

    bool HasHit;

    Integer Damage;

    Integer DifficultyClass;

    void Reset() {AttackRoll = 0; HasHit = false; Damage = 0; DifficultyClass = 0;}
};

static SpellResult spellResult;

void SpellCaster::ComputeDifficultyClass(Combatant& combatant)
{
    spellResult.DifficultyClass = 8 + combatant.character->GetSpellCastingAbility().Modifier + combatant.character->GetMagicProficiencyBonus();
}

void SpellCaster::RollDamage(Combatant &combatant, const Spell & spell)
{
    Integer damage = 0;
    if(spellResult.HasHit)
    {
        damage = utility::GetRandom(1, spell.HitDice);
        combatant.target->SufferDamage(damage);
    }

    spellResult.Damage = damage;
}

void SpellCaster::RollHealing(Combatant &combatant, const Spell & spell)
{
    Integer damage = combatant.character->GetSpellCastingAbility().Modifier + utility::GetRandom(1, spell.HitDice);
    combatant.GetTarget()->HealDamage(damage);

    spellResult.Damage = damage;
}

void SpellCaster::RollAttack(Combatant &combatant)
{
    bool hasDisadvantage = combatant.HasDisadvantage();

    auto attackRoll = utility::GetRandom(1, 20);
    if(hasDisadvantage)
    {
        auto newRoll = utility::GetRandom(1, 20);
        if(newRoll < attackRoll)
        {
            attackRoll = newRoll;
        }
    }

    attackRoll += combatant.character->GetSpellCastingAbility().Modifier + combatant.character->GetMagicProficiencyBonus();

    spellResult.HasHit = attackRoll >= combatant.target->armorClass;
    spellResult.AttackRoll = attackRoll;
}

CharacterActionData SpellCaster::ApplyFrostRay(Combatant &combatant, const Spell & spell)
{
    ComputeDifficultyClass(combatant);

    RollAttack(combatant);

    RollDamage(combatant, spell);

    if(spellResult.HasHit)
    {
        auto condition = Condition(ConditionTypes::CHILL, combatant.target, spellResult.DifficultyClass);
        combatant.target->AddCondition(condition);
    }
}

CharacterActionData SpellCaster::ApplyShockingGrasp(Combatant & character, const Spell & spell)
{
    RollAttack(character);

    RollDamage(character, spell);   
}

CharacterActionData SpellCaster::ApplySacredFlame(Combatant &combatant, const Spell & spell)
{
    ComputeDifficultyClass(combatant);

    auto reflexSavingThrow = utility::GetRandom(1, 20) + combatant.character->GetReflexSaveBonus();
    if(reflexSavingThrow < spellResult.DifficultyClass) 
    {
        spellResult.HasHit = true;
    }

    RollDamage(combatant, spell);
}

CharacterActionData SpellCaster::ApplyCureWounds(Combatant &combatant, const Spell & spell)
{
    RollHealing(combatant, spell);
}

CharacterActionData SpellCaster::ApplyEffect(Combatant &combatant, const Spell & spell)
{
    spellResult.Reset();

    switch(spell.Type)
    {
        case SpellTypes::FROST_RAY:
            ApplyFrostRay(combatant, spell);
            break;
        case SpellTypes::SHOCKING_GRASP:
            ApplyShockingGrasp(combatant, spell);
            break;
        case SpellTypes::SACRED_FLAME:
            ApplySacredFlame(combatant, spell);
            break;
        case SpellTypes::CURE_WOUNDS:
            ApplyCureWounds(combatant, spell);
            break;
        default:
            return CharacterActionData();    
    }

    if(spell.Level > 0)
    {
        auto slot = combatant.character->spellSlots.Get(spell.Level - 1);
        slot->Current--;
    }

    return CharacterActionData(
        CharacterActions::CAST_SPELL, 
        &combatant, 
        spellResult.AttackRoll, 
        combatant.target->armorClass, 
        spellResult.Damage,
        spellResult.HasHit
        );
}