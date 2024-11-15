#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/Utility/Types.h"

namespace world::character
{
    struct SpellSlot
    {
        Integer Current;

        Integer Maximum;

        SpellSlot() : Maximum(0), Current(0) {}

        SpellSlot(Integer maximum) : Maximum(maximum), Current(maximum) {}

        void Reset() {Current = Maximum;}
    };

    struct Spell
    {
        SpellTypes Type;

        Integer Level;

        utility::RollDies HitDice;

        Integer RollCount;

        Integer Range;

        Word Name;

        bool IsOffensive;

        bool IsAttack;

        bool IsBonusAction;

        struct SaveType
        {
            bool HasSave;

            SavingThrows Type;

            SaveType() : HasSave(false) {}

            SaveType(SavingThrows type) : HasSave(true), Type(type) {}
        } SaveType;

        //const Phrase & Description;

        Spell() {}

        Spell(SpellTypes type) : Type(type) {}

        Spell(SpellTypes type, Integer level, utility::RollDies hitDice, Integer rollCount, Integer range, Word name, bool isOffensive, bool isAttack, bool isBonusAction) :
            Type(type), Level(level), HitDice(hitDice), RollCount(rollCount), Range(range), Name(name), IsOffensive(isOffensive), IsAttack(isAttack), IsBonusAction(isBonusAction), SaveType() {}

        Spell(SpellTypes type, Integer level, utility::RollDies hitDice, Integer rollCount, Integer range, Word name, bool isOffensive, bool isAttack, bool isBonusAction, struct SaveType saveType) :
            Type(type), Level(level), HitDice(hitDice), RollCount(rollCount), Range(range), Name(name), IsOffensive(isOffensive), IsAttack(isAttack), IsBonusAction(isBonusAction), SaveType(saveType) {}

        bool operator== (const Spell &other) {return other.Type == this->Type;}
    };

    class SpellFactory
    {
    public:
        static Spell BuildFrostRay()
        {
            return {SpellTypes::FROST_RAY, 0, utility::RollDies::D8, 1, 12, "Frost Ray", true, true, false, SavingThrows::FORTITUDE};
        }

        static Spell BuildShockingGrasp()
        {
            return {SpellTypes::SHOCKING_GRASP, 0, utility::RollDies::D8, 1, 1, "Shocking Grasp", true, true, false, SavingThrows::REFLEX};
        }

        static Spell BuildSacredFlame()
        {
            return {SpellTypes::SACRED_FLAME, 0, utility::RollDies::D8, 1, 12, "Sacred Flame", true, true, false, SavingThrows::REFLEX};
        }

        static Spell BuildFireBolt()
        {
            return {SpellTypes::FIRE_BOLT, 0, utility::RollDies::D10, 1, 24, "Fire Bolt", true, true, false, SavingThrows::REFLEX};
        }

        static Spell BuildMagicMissile()
        {
            return {SpellTypes::MAGIC_MISSILE, 1, utility::RollDies::D4, 1, 24, "Magic Missile", true, false, false};
        }

        static Spell BuildCureWounds()
        {
            return {SpellTypes::CURE_WOUNDS, 1, utility::RollDies::D8, 1, 1, "Cure Wounds", false, false, false};
        }

        static Spell BuildHealingWord()
        {
            return {SpellTypes::HEALING_WORD, 1, utility::RollDies::D4, 1, 12, "Healing Word", false, false, true};
        }

        static Spell BuildBless()
        {
            return {SpellTypes::BLESS, 1, utility::RollDies::D4, 1, 5, "Bless", false, false, false};
        }
    };
}