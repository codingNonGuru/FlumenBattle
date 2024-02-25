#pragma once

#include "FlumenBattle/Race.h"

struct Dwarf : Race
{
    using Race::Race;

    void ApplyModifiers(world::settlement::Settlement &) const override;
};

struct Human : Race
{
    using Race::Race;

    //void ApplyModifiers(world::settlement::Settlement &) const override;
};

struct Elf : Race
{
    using Race::Race;

    void ApplyModifiers(world::settlement::Settlement &) const override;
};

struct Orc : Race
{
    using Race::Race;

    //void ApplyModifiers(world::settlement::Settlement &) const override;
};

struct Goblin : Race
{
    using Race::Race;

    void ApplyModifiers(world::settlement::Settlement &) const override;
};