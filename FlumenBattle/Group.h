#pragma once

#include "FlumenCore/Conventions.hpp"
#include "FlumenEngine/Utility/Color.hpp"

class Character;

class Group
{
    Color color;

    Array <Character> characters;

    friend class CharacterFactory;

    friend class GroupFactory;

    friend class CharacterModel;

    friend class BattleScene;

    Group();

    void Initialize(Integer size, Color color);

public:
    const Color& GetColor() const {return color;} 
};