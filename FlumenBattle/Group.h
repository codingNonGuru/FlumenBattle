#pragma once

#include "FlumenCore/Conventions.hpp"
#include "FlumenEngine/Utility/Color.hpp"

class Character;
class BattleTile;

class Group
{
    friend class CharacterFactory;

    friend class GroupFactory;

    friend class BattleInterface;

    friend class BattleScene;

    friend class container::Array<Group>;

    BattleTile * tile;
    
    Color color;

    Array <Character> characters;

    Group();

    void Initialize(Integer size, Color color);

public:
    const Color& GetColor() const {return color;} 

    Array <Character> & GetCharacters() {return characters;}
};