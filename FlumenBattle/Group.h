#pragma once

//#include "FlumenCore/Conventions.hpp"

#include "FlumenEngine/Utility/Color.hpp"

class Character;
class Battle;
enum class RaceTypes;

class Group
{
    friend class container::Array <Group>;

    friend class CharacterFactory;

    friend class GroupFactory;

    friend class BattleInterface;

    friend class WorldScene;

    friend class Character;

    friend class CombatGroup;

    Battle *battle;

    Color color;

    Array <Character> characters;

    Group();

    void Initialize(Integer, Color, RaceTypes);

public:
    const Color& GetColor() const {return color;} 

    Array <Character> & GetCharacters() {return characters;}
};