#pragma once

#include "FlumenBattle/Types.hpp"

namespace world::character
{
    struct CharacterAction
    {
        CharacterActions Type;

        CharacterAction() : Type() {}

        CharacterAction(CharacterActions type) : Type(type) {}

        bool operator== (const CharacterAction &other) {return this->Type == other.Type;}
    };
}