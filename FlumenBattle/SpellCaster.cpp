#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenBattle/SpellCaster.h"
#include "FlumenBattle/Spell.h"
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

void SpellCaster::ComputeDifficultyClass(Character& character)
{
    spellResult.DifficultyClass = 8 + character.GetSpellCastingAbility().Modifier + character.GetMagicProficiencyBonus();
}

void SpellCaster::RollDamage(Character & character, const Spell & spell)
{
    Integer damage = 0;
    if(spellResult.HasHit)
    {
        damage = utility::GetRandom(1, spell.HitDice);
        character.target->SufferDamage(damage);
    }

    spellResult.Damage = damage;
}

void SpellCaster::RollHealing(Character & character, const Spell & spell)
{
    Integer damage = character.GetSpellCastingAbility().Modifier + utility::GetRandom(1, spell.HitDice);
    character.target->HealDamage(damage);

    spellResult.Damage = damage;
}

void SpellCaster::RollAttack(Character & character)
{
    bool hasDisadvantage = character.HasDisadvantage();

    auto attackRoll = utility::GetRandom(1, 20);
    if(hasDisadvantage)
    {
        auto newRoll = utility::GetRandom(1, 20);
        if(newRoll < attackRoll)
        {
            attackRoll = newRoll;
        }
    }

    attackRoll += character.GetSpellCastingAbility().Modifier + character.GetMagicProficiencyBonus();

    spellResult.HasHit = attackRoll >= character.target->armorClass;
    spellResult.AttackRoll = attackRoll;
}

CharacterActionData SpellCaster::ApplyFrostRay(Character & character, const Spell & spell)
{
    ComputeDifficultyClass(character);

    RollAttack(character);

    RollDamage(character, spell);

    if(spellResult.HasHit)
    {
        auto condition = Condition(ConditionTypes::CHILL, character.target, spellResult.DifficultyClass);
        character.target->AddCondition(condition);
    }
}

CharacterActionData SpellCaster::ApplyShockingGrasp(Character & character, const Spell & spell)
{
    RollAttack(character);

    RollDamage(character, spell);   
}

CharacterActionData SpellCaster::ApplySacredFlame(Character & character, const Spell & spell)
{
    ComputeDifficultyClass(character);

    auto reflexSavingThrow = utility::GetRandom(1, 20) + character.GetReflexSaveBonus();
    if(reflexSavingThrow < spellResult.DifficultyClass) 
    {
        spellResult.HasHit = true;
    }

    RollDamage(character, spell);
}

CharacterActionData SpellCaster::ApplyCureWounds(Character & character, const Spell & spell)
{
    RollHealing(character, spell);
}

CharacterActionData SpellCaster::ApplyEffect(Character & character, const Spell & spell)
{
    spellResult.Reset();

    switch(spell.Type)
    {
        case SpellTypes::FROST_RAY:
            ApplyFrostRay(character, spell);
            break;
        case SpellTypes::SHOCKING_GRASP:
            ApplyShockingGrasp(character, spell);
            break;
        case SpellTypes::SACRED_FLAME:
            ApplySacredFlame(character, spell);
            break;
        case SpellTypes::CURE_WOUNDS:
            ApplyCureWounds(character, spell);
            break;
        default:
            return CharacterActionData();    
    }

    if(spell.Level > 0)
    {
        auto slot = character.spellSlots.Get(spell.Level - 1);
        slot->Current--;
    }

    return CharacterActionData(
        CharacterActions::CAST_SPELL, 
        &character, 
        spellResult.AttackRoll, 
        character.target->armorClass, 
        spellResult.Damage,
        spellResult.HasHit
        );
}