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

    Word name;

    Battle *battle;

    Color color;

    Array <Character> characters;

    Group();

    void Initialize(Word, Integer, Color, RaceTypes);

public:
    bool IsAlive();

    const Color& GetColor() const {return color;} 

    Word GetName() const {return name;}

    Array <Character> & GetCharacters() {return characters;}

    void Rest();
};