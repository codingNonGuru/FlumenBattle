#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenBattle/Battle/SpellCaster.h"
#include "FlumenBattle/World/Character/Spell.h"
#include "FlumenBattle/Battle/Combatant.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/Battle/CharacterActionData.h"
#include "FlumenBattle/World/Character/Condition.h"
#include "FlumenBattle/Utility/Utility.h"
#include "FlumenBattle/Battle/BattleScene.h"
#include "FlumenBattle/Battle/BattleTile.h"
#include "FlumenBattle/Battle/HumanController.h"
#include "FlumenBattle/Battle/BattleMap.h"

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

    container::Array <SpellEffect> Effects {32};

    void Reset() {AttackRoll = 0; HasHit = false; IsCritical = false; Damage = 0; DifficultyClass = 0; Effects.Reset();}
};

static SpellResult spellResult;

void SpellCaster::ComputeDifficultyClass(Combatant& caster)
{
    spellResult.DifficultyClass = 8 + caster.GetCharacter()->GetSpellCastingAbility().Modifier + caster.GetCharacter()->GetMagicProficiencyBonus();
}

void SpellCaster::RollDamage(Combatant &target, const Spell & spell, bool hasThrownSave)
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
        target.SufferDamage(damage);
    }

    if(hasThrownSave == true)
    {
        auto lastEffect = spellResult.Effects.GetLast();
        lastEffect->Damage = damage;
    }
    else
    {
        spellResult.Damage = damage;
    }
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

    auto result = utility::RollD20Dice(combatant.GetTarget()->GetArmorClass(), attackBonus);

    spellResult.IsCritical = result.IsCriticalSuccess() || result.IsCriticalFailure();

    spellResult.HasHit = result.IsAnySuccess();
    spellResult.AttackRoll = result.Roll;
}

void SpellCaster::RollSavingThrow(Combatant &combatant, const Spell &spell)
{
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

    *spellResult.Effects.Allocate() = {{&combatant, savingThrow}, 0};
}

container::Array <BattleTile *> tiles = container::Array <BattleTile *> (256);

container::Array <BattleTile *> &SpellCaster::GetAffectedTiles(Combatant &caster, const Spell &spell, BattleTile *targetTile)
{
    tiles.Reset();

    if(spell.Type == SpellTypes::BURNING_HANDS)
    {
        if(caster.GetTile() == targetTile)
            return tiles;

        auto distance = targetTile->GetDistanceTo(*caster.GetTile());

        auto ratio = (float) spell.EffectArea.Size / (float) distance;
        
        Float3 delta = targetTile->Coordinates - caster.GetTile()->Coordinates;
        auto newCoords = utility::RoundHexCoords(Float3(caster.GetTile()->Coordinates) + delta * ratio);

        static auto endTiles = container::Array <BattleTile *> (64);

        endTiles.Reset();

        auto centerEndTile = BattleScene::Get()->GetBattleMap()->GetTile(newCoords);

        auto range = (spell.EffectArea.Breadth - 1) / 2;

        auto &neighbours = centerEndTile->GetNearbyTiles(range);
        for(auto &neighbour : neighbours)
        {
            auto distanceToNeighbour = caster.GetTile()->GetDistanceTo(*neighbour);
            if(distanceToNeighbour != spell.EffectArea.Size)
                continue;

            *endTiles.Allocate() = neighbour;
        }

        for(auto &endTile : endTiles)
        {
            for (int i = 0; i <= spell.EffectArea.Size; ++i)
            {
                auto factor = float(i) / float(spell.EffectArea.Size);
                auto coords = utility::RoundHexCoords(utility::InterpolateHexly(caster.GetTile()->Coordinates, endTile->Coordinates, factor));

                auto tile = BattleScene::Get()->GetBattleMap()->GetTile(coords);
                if(tiles.Find(tile) != nullptr)
                    continue;

                if(tile == caster.GetTile())
                    continue;

                *tiles.Allocate() = tile;
            }
        }
    }
    else if(spell.Type == SpellTypes::FIRE_BALL)
    {
        auto &nearbyTiles = targetTile->GetNearbyTiles(spell.EffectArea.Size);

        for(auto &tile : nearbyTiles)
        {
            *tiles.Allocate() = tile;
        }
    }

    return tiles;
}

CharacterActionData SpellCaster::ApplyFrostRay(Combatant &caster, const Spell & spell)
{
    RollAttack(caster);

    if(spellResult.HasHit)
    {
        ComputeDifficultyClass(caster);
        
        RollSavingThrow(*caster.GetTarget(), spell);

        RollDamage(*caster.GetTarget(), spell, true);

        BattleScene::Get()->AddCondition(caster.GetTarget(), {world::character::Conditions::HOBBLED, 3, 1});
    }
}

CharacterActionData SpellCaster::ApplyShockingGrasp(Combatant &caster, const Spell & spell)
{
    RollAttack(caster);

    RollDamage(*caster.GetTarget(), spell);   
}

CharacterActionData SpellCaster::ApplySacredFlame(Combatant &caster, const Spell & spell)
{
    ComputeDifficultyClass(caster);

    RollSavingThrow(*caster.GetTarget(), spell);

    RollDamage(*caster.GetTarget(), spell, true);
}

CharacterActionData SpellCaster::ApplyFireBolt(Combatant &caster, const Spell & spell)
{
    RollAttack(caster);

    RollDamage(*caster.GetTarget(), spell);   
}

CharacterActionData SpellCaster::ApplyCureWounds(Combatant &caster, const Spell &spell)
{
    RollHealing(caster, spell);
}

CharacterActionData SpellCaster::ApplyHealingWord(Combatant &caster, const Spell &spell)
{
    RollHealing(caster, spell);
}

CharacterActionData SpellCaster::ApplyBless(Combatant &caster, const Spell &spell)
{
    BattleScene::Get()->AddCondition(caster.GetTarget(), {world::character::Conditions::BLESSED, 3, 1});
}

CharacterActionData SpellCaster::ApplyFireball(Combatant &caster, BattleTile &tile, const Spell &spell)
{
    ComputeDifficultyClass(caster);

    auto &tiles = GetAffectedTiles(caster, spell, &tile);

    for(auto &tile : tiles)
    {
        if(tile->Combatant == nullptr)
            continue;

        RollSavingThrow(*tile->Combatant, spell);

        RollDamage(*tile->Combatant, spell, true);
    }
}

CharacterActionData SpellCaster::ApplyBurningHands(Combatant &caster, BattleTile &tile, const Spell &spell)
{
    ComputeDifficultyClass(caster);

    auto &tiles = GetAffectedTiles(caster, spell, &tile);

    for(auto &tile : tiles)
    {
        if(tile->Combatant == nullptr)
            continue;

        RollSavingThrow(*tile->Combatant, spell);

        RollDamage(*tile->Combatant, spell, true);
    }
}

CharacterActionData SpellCaster::ApplyEffect(Combatant *caster, BattleTile *tile, const Spell &spell)
{
    spellResult.Reset();

    switch(spell.Type)
    {
        case SpellTypes::FROST_RAY:
            ApplyFrostRay(*caster, spell);
            break;
        case SpellTypes::SHOCKING_GRASP:
            ApplyShockingGrasp(*caster, spell);
            break;
        case SpellTypes::SACRED_FLAME:
            ApplySacredFlame(*caster, spell);
            break;
        case SpellTypes::FIRE_BOLT:
            ApplyFireBolt(*caster, spell);
            break;
        case SpellTypes::CURE_WOUNDS:
            ApplyCureWounds(*caster, spell);
            break;
        case SpellTypes::HEALING_WORD:
            ApplyHealingWord(*caster, spell);
            break;
        case SpellTypes::BLESS:
            ApplyBless(*caster, spell);
            break;
        case SpellTypes::FIRE_BALL:
            ApplyFireball(*caster, *tile, spell);
            break;
        case SpellTypes::BURNING_HANDS:
            ApplyBurningHands(*caster, *tile, spell);
            break;
        default:
            return CharacterActionData();    
    }

    if(spell.Level > 0)
    {
        //auto slot = combatant.character->spellSlots.Get(spell.Level - 1);
        //slot->Current--;
        caster->GetCharacter()->spellUseCount--;
    }

    auto actionData = CharacterActionData(
        world::character::CharacterActions::CAST_SPELL, 
        caster, 
        spellResult.AttackRoll, 
        tile == nullptr ? caster->GetTarget()->GetArmorClass() : 0, 
        spellResult.Damage,
        spellResult.HasHit,
        &spellResult.Effects,
        tile != nullptr
        );

    return actionData;
}