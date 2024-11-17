#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenBattle/Battle/SpellCaster.h"
#include "FlumenBattle/World/Character/Spell.h"
#include "FlumenBattle/Battle/Combatant.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/Battle/CharacterActionData.h"
#include "FlumenBattle/World/Character/Condition.h"
#include "FlumenBattle/Utility/Utility.h"
#include "FlumenBattle/Battle/BattleScene.h"

using namespace battle;
using namespace world::character;

struct SpellResult 
{
    Integer AttackRoll;

    bool HasHit;

    bool IsCritical;

    Integer Damage;

    Integer DifficultyClass;

    utility::Success Success;

    void Reset() {AttackRoll = 0; HasHit = false; IsCritical = false; Damage = 0; DifficultyClass = 0;}
};

static SpellResult spellResult;

void SpellCaster::ComputeDifficultyClass(Combatant& combatant)
{
    spellResult.DifficultyClass = 8 + combatant.GetCharacter()->GetSpellCastingAbility().Modifier + combatant.GetCharacter()->GetMagicProficiencyBonus();
}

void SpellCaster::RollDamage(Combatant &combatant, const Spell & spell)
{
    Integer damage = 0;
    if(spellResult.HasHit)
    {
        damage += utility::RollDice({spell.HitDice, spell.RollCount});

        if(spellResult.IsCritical)
        {
            damage += (int)spell.HitDice * spell.RollCount;
        }
    }
    else
    {
        if(spellResult.IsCritical == false)
        {
            damage += utility::RollDice({spell.HitDice, spell.RollCount}) / 2;
        }
    }

    if(spellResult.HasHit == true || (spellResult.HasHit == false && spellResult.IsCritical == false))
    {
        combatant.GetTarget()->SufferDamage(damage);
    }

    spellResult.Damage = damage;
}

void SpellCaster::RollHealing(Combatant &combatant, const Spell & spell)
{
    Integer damage = combatant.GetCharacter()->GetSpellCastingAbility().Modifier + utility::RollDice({spell.HitDice, 1});
    combatant.GetTarget()->HealDamage(damage);

    spellResult.Damage = damage;
}

void SpellCaster::RollAttack(Combatant &combatant)
{
    auto attackBonus = combatant.GetCharacter()->GetSpellCastingAbility().Modifier + combatant.GetCharacter()->GetMagicProficiencyBonus();

    auto result = utility::RollD20Dice(combatant.GetTarget()->armorClass, attackBonus);

    spellResult.IsCritical = result.IsCriticalSuccess() || result.IsCriticalFailure();

    spellResult.HasHit = result.IsAnySuccess();
    spellResult.AttackRoll = result.Roll;
}

void SpellCaster::RollSavingThrow(Combatant &combatant, const Spell &spell)
{
    ComputeDifficultyClass(combatant);

    auto savingThrow = [&]
    {
        switch(spell.SaveType.Type)
        {
        case SavingThrows::REFLEX:
            return utility::RollD20Dice(spellResult.DifficultyClass, combatant.GetCharacter()->GetReflexSaveBonus());
        case SavingThrows::WILL:
            return utility::RollD20Dice(spellResult.DifficultyClass, combatant.GetCharacter()->GetWillSaveBonus());
        case SavingThrows::FORTITUDE:
            return utility::RollD20Dice(spellResult.DifficultyClass, combatant.GetCharacter()->GetFortitudeSaveBonus());
        }
    } ();

    spellResult.HasHit = savingThrow.IsAnyFailure();
    spellResult.IsCritical = savingThrow.IsCriticalFailure() || savingThrow.IsCriticalSuccess();
}

CharacterActionData SpellCaster::ApplyFrostRay(Combatant &combatant, const Spell & spell)
{
    RollAttack(combatant);

    if(spellResult.HasHit)
    {
        RollDamage(combatant, spell);

        BattleScene::Get()->AddCondition(combatant.GetTarget(), {world::character::Conditions::HOBBLED, 3, 1});
    }
}

CharacterActionData SpellCaster::ApplyShockingGrasp(Combatant &combatant, const Spell & spell)
{
    RollAttack(combatant);

    RollDamage(combatant, spell);   
}

CharacterActionData SpellCaster::ApplySacredFlame(Combatant &combatant, const Spell & spell)
{
    RollSavingThrow(combatant, spell);

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
    BattleScene::Get()->AddCondition(combatant.GetTarget(), {world::character::Conditions::BLESSED, 3, 1});
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
        combatant.GetCharacter()->spellUseCount--;
    }

    return CharacterActionData(
        world::character::CharacterActions::CAST_SPELL, 
        &combatant, 
        spellResult.AttackRoll, 
        combatant.GetTarget()->armorClass, 
        spellResult.Damage,
        spellResult.HasHit
        );
}