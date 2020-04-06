#pragma once

#include "FlumenCore/Conventions.hpp"

class Character;

class Group
{
    Array <Character> characters;

    Group() {}

    void Initialize();

    friend class CharacterFactory;

    friend class GroupFactory;

    friend class CharacterModel;
};