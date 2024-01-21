#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenBattle/Battle/SpellCaster.h"
#include "FlumenBattle/World/Character/Spell.h"
#include "FlumenBattle/Battle/Combatant.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/Battle/CharacterActionData.h"
#include "FlumenBattle/World/Character/Condition.h"

using namespace battle;
using namespace world::character;

struct SpellResult 
{
    Integer AttackRoll;

    bool HasHit;

    bool IsCritical;

    Integer Damage;

    Integer DifficultyClass;

    void Reset() {AttackRoll = 0; HasHit = false; IsCritical = false; Damage = 0; DifficultyClass = 0;}
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
        for(Index i = 0; i < spell.RollCount; ++i)
        {
            damage += utility::GetRandom(1, spell.HitDice);
        }

        if(spellResult.IsCritical)
        {
            damage += spell.HitDice * spell.RollCount;
        }

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

    spellResult.IsCritical = attackRoll == 20;

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
        //auto condition = world::character::Condition(ConditionTypes::CHILL, combatant.target, spellResult.DifficultyClass);
        combatant.target->AddCondition(world::character::Conditions::HOBBLED, combatant.target->character);
    }
}

CharacterActionData SpellCaster::ApplyShockingGrasp(Combatant &combatant, const Spell & spell)
{
    RollAttack(combatant);

    RollDamage(combatant, spell);   
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

CharacterActionData SpellCaster::ApplyFireBolt(Combatant &combatant, const Spell & spell)
{
    RollAttack(combatant);

    RollDamage(combatant, spell);   
}

CharacterActionData SpellCaster::ApplyCureWounds(Combatant &combatant, const Spell &spell)
{
    RollHealing(combatant, spell);
}

CharacterActionData SpellCaster::ApplyHealingWord(Combatant &combatant, const Spell &spell)
{
    RollHealing(combatant, spell);
}

CharacterActionData SpellCaster::ApplyBless(Combatant &combatant, const Spell &spell)
{
    combatant.target->GetCharacter()->AddCondition({world::character::Conditions::BLESSED, 3, 1});
}

CharacterActionData SpellCaster::ApplyEffect(Combatant &combatant, const Spell &spell)
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
        case SpellTypes::FIRE_BOLT:
            ApplyFireBolt(combatant, spell);
            break;
        case SpellTypes::CURE_WOUNDS:
            ApplyCureWounds(combatant, spell);
            break;
        case SpellTypes::HEALING_WORD:
            ApplyHealingWord(combatant, spell);
            break;
        case SpellTypes::BLESS:
            ApplyBless(combatant, spell);
            break;
        default:
            return CharacterActionData();    
    }

    if(spell.Level > 0)
    {
        //auto slot = combatant.character->spellSlots.Get(spell.Level - 1);
        //slot->Current--;
        combatant.character->spellUseCount--;
    }

    return CharacterActionData(
        world::character::CharacterActions::CAST_SPELL, 
        &combatant, 
        spellResult.AttackRoll, 
        combatant.target->armorClass, 
        spellResult.Damage,
        spellResult.HasHit
        );
}