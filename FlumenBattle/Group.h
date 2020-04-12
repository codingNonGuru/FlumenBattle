#pragma once

#include "FlumenCore/Conventions.hpp"
#include "FlumenEngine/Utility/Color.hpp"

class Character;

class Group
{
    friend class CharacterFactory;

    friend class GroupFactory;

    friend class CharacterModel;

    friend class BattleScene;

    friend class container::Array<Group>;
    
    Color color;

    Array <Character> characters;

    Group();

    void Initialize(Integer size, Color color);

public:
    const Color& GetColor() const {return color;} 
};