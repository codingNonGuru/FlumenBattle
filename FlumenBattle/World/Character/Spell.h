#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/Utility/Types.h"

namespace world::character
{
    struct EffectArea
    {
        enum class Shapes {CIRCLE, LINE, CONE, NONE} Shape {Shapes::NONE};

        int Size;

        int Breadth;
    };

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
        struct SaveType
        {
            bool HasSave;

            SavingThrows Type;

            SaveType() : HasSave(false) {}

            SaveType(SavingThrows type) : HasSave(true), Type(type) {}
        };

        SpellTypes Type;

        Integer Level;

        utility::RollDies HitDice;

        Integer RollCount;

        Integer Range;

        Word Name;

        bool IsOffensive;

        bool IsAttack;

        bool IsBonusAction;

        struct SaveType SaveType;

        bool CanTargetTiles {false};

        struct EffectArea EffectArea;

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

        bool HasAreaOfEffect() const {return EffectArea.Shape != EffectArea::Shapes::NONE;}
    };

    class SpellFactory
    {
    public:
        static const Spell BuildFrostRay()
        {
            static Spell spell = {SpellTypes::FROST_RAY, 0, utility::RollDies::D8, 1, 12, "Frost Ray", true, true, false, false, SavingThrows::FORTITUDE};
            return spell;
        }

        static const Spell BuildShockingGrasp()
        {
            static Spell spell = {SpellTypes::SHOCKING_GRASP, 0, utility::RollDies::D8, 1, 1, "Shocking Grasp", true, true, false, false, SavingThrows::REFLEX};
            return spell;
        }

        static const Spell BuildSacredFlame()
        {
            static Spell spell = {SpellTypes::SACRED_FLAME, 0, utility::RollDies::D8, 1, 12, "Sacred Flame", true, true, false, false, SavingThrows::REFLEX};
            return spell;
        }

        static const Spell BuildFireBolt()
        {
            static Spell spell = {SpellTypes::FIRE_BOLT, 0, utility::RollDies::D10, 1, 24, "Fire Bolt", true, true, false, false};
            return spell;
        }

        static const Spell BuildMagicMissile()
        {
            static Spell spell = {SpellTypes::MAGIC_MISSILE, 1, utility::RollDies::D4, 1, 24, "Magic Missile", true, false, false, false};
            return spell;
        }

        static const Spell BuildCureWounds()
        {
            static Spell spell = {SpellTypes::CURE_WOUNDS, 1, utility::RollDies::D8, 1, 1, "Cure Wounds", false, false, false, false};
            return spell;
        }

        static const Spell BuildHealingWord()
        {
            static Spell spell = {SpellTypes::HEALING_WORD, 1, utility::RollDies::D4, 1, 12, "Healing Word", false, false, true, false};
            return spell;
        }

        static const Spell BuildBless()
        {
            static Spell spell = {SpellTypes::BLESS, 1, utility::RollDies::D4, 1, 5, "Bless", false, false, false, false};
            return spell;
        }

        static const Spell BuildFireball()
        {
            static Spell spell = {SpellTypes::FIRE_BALL, 1, utility::RollDies::D6, 3, 12, "Fireball", true, false, false, true, SavingThrows::REFLEX};
            spell.EffectArea = {EffectArea::Shapes::CIRCLE, 2};
            return spell;
        }

        static const Spell BuildBurningHands()
        {
            static Spell spell = {SpellTypes::BURNING_HANDS, 1, utility::RollDies::D8, 2, 6, "Burning Hands", true, false, false, true, SavingThrows::REFLEX};
            spell.EffectArea = {EffectArea::Shapes::CONE, 6, 5};
            return spell;
        }

        static const Spell Build(SpellTypes type)
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
            case SpellTypes::BURNING_HANDS:
                return BuildBurningHands();
            }
        }
    };
}