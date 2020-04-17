#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenBattle/Types.hpp"

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

    Integer HitDice;

    Integer RollCount;

    Integer Range;

    Word Name;

    bool IsOffensive;

    bool IsAttack;

    //const Phrase & Description;

    Spell() {}

    Spell(SpellTypes type) : Type(type) {}

    Spell(SpellTypes type, Integer level, Integer hitDice, Integer rollCount, Integer range, Word name, bool isOffensive, bool isAttack) :
        Type(type), Level(level), HitDice(hitDice), RollCount(rollCount), Range(range), Name(name), IsOffensive(isOffensive), IsAttack(isAttack)
        {}

    bool operator== (const Spell &other) {return other.Type == this->Type;}
};

class SpellFactory
{
public:
    static Spell BuildFrostRay()
    {
        return {SpellTypes::FROST_RAY, 0, 8, 1, 12, "Frost Ray", true, true};
    }

    static Spell BuildShockingGrasp()
    {
        return {SpellTypes::SHOCKING_GRASP, 0, 8, 1, 1, "Shocking Grasp", true, true};
    }

    static Spell BuildSacredFlame()
    {
        return {SpellTypes::SACRED_FLAME, 0, 8, 1, 12, "Sacred Flame", true, true};
    }

    static Spell BuildMagicMissile()
    {
        return {SpellTypes::MAGIC_MISSILE, 1, 4, 1, 24, "Magic Missile", true, false};
    }

    static Spell BuildCureWounds()
    {
        return {SpellTypes::CURE_WOUNDS, 1, 8, 1, 1, "Cure Wounds", false, false};
    }
};