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

        bool CanTargetTiles {false};

        //const Phrase & Description;

        Spell() {}

        Spell(SpellTypes type) : Type(type) {}

        Spell(SpellTypes type, 
            Integer level, 
            utility::RollDies hitDice, 
            Integer rollCount, 
            Integer range, 
            Word name, 
            bool isOffensive, 
            bool isAttack, 
            bool isBonusAction,
            bool canTargetTiles) :
            Type(type), Level(level), HitDice(hitDice), RollCount(rollCount), Range(range), Name(name), IsOffensive(isOffensive), IsAttack(isAttack), IsBonusAction(isBonusAction), CanTargetTiles(canTargetTiles), SaveType() {}

        Spell(SpellTypes type, 
            Integer level, 
            utility::RollDies hitDice, 
            Integer rollCount, 
            Integer range, 
            Word name, 
            bool isOffensive, 
            bool isAttack, 
            bool isBonusAction,
            bool canTargetTiles,
            struct SaveType saveType) :
            Type(type), Level(level), HitDice(hitDice), RollCount(rollCount), Range(range), Name(name), IsOffensive(isOffensive), IsAttack(isAttack), IsBonusAction(isBonusAction), SaveType(saveType), CanTargetTiles(canTargetTiles) {}

        bool operator== (const Spell &other) {return other.Type == this->Type;}
    };

    class SpellFactory
    {
    public:
        static Spell BuildFrostRay()
        {
            return {SpellTypes::FROST_RAY, 0, utility::RollDies::D8, 1, 12, "Frost Ray", true, true, false, false, SavingThrows::FORTITUDE};
        }

        static Spell BuildShockingGrasp()
        {
            return {SpellTypes::SHOCKING_GRASP, 0, utility::RollDies::D8, 1, 1, "Shocking Grasp", true, true, false, false, SavingThrows::REFLEX};
        }

        static Spell BuildSacredFlame()
        {
            return {SpellTypes::SACRED_FLAME, 0, utility::RollDies::D8, 1, 12, "Sacred Flame", true, true, false, false, SavingThrows::REFLEX};
        }

        static Spell BuildFireBolt()
        {
            return {SpellTypes::FIRE_BOLT, 0, utility::RollDies::D10, 1, 24, "Fire Bolt", true, true, false, false, SavingThrows::REFLEX};
        }

        static Spell BuildMagicMissile()
        {
            return {SpellTypes::MAGIC_MISSILE, 1, utility::RollDies::D4, 1, 24, "Magic Missile", true, false, false, false};
        }

        static Spell BuildCureWounds()
        {
            return {SpellTypes::CURE_WOUNDS, 1, utility::RollDies::D8, 1, 1, "Cure Wounds", false, false, false, false};
        }

        static Spell BuildHealingWord()
        {
            return {SpellTypes::HEALING_WORD, 1, utility::RollDies::D4, 1, 12, "Healing Word", false, false, true, false};
        }

        static Spell BuildBless()
        {
            return {SpellTypes::BLESS, 1, utility::RollDies::D4, 1, 5, "Bless", false, false, false, false};
        }

        static Spell BuildFireball()
        {
            return {SpellTypes::FIRE_BALL, 1, utility::RollDies::D6, 3, 12, "Fireball", true, false, false, true, SavingThrows::REFLEX};
        }

        static Spell Build(SpellTypes type)
        {
            switch(type)
            {
            case SpellTypes::BLESS:
                return BuildBless();
            case SpellTypes::CURE_WOUNDS:
                return BuildCureWounds();
            case SpellTypes::FIRE_BALL:
                return BuildFireball();
            case SpellTypes::FIRE_BOLT:
                return BuildFireBolt();
            case SpellTypes::FROST_RAY:
                return BuildFrostRay();
            case SpellTypes::HEALING_WORD:
                return BuildHealingWord();
            case SpellTypes::MAGIC_MISSILE:
                return BuildMagicMissile();
            case SpellTypes::SACRED_FLAME:
                return BuildSacredFlame();
            case SpellTypes::SHOCKING_GRASP:
                return BuildShockingGrasp();
            }
        }
    };
}