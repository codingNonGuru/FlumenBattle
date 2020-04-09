#pragma once

#include "FlumenBattle/FlumenTypes/Types.hpp"

#include "Types.hpp"

class CharacterFactory;
class CharacterInfo;
class Group;
class BattleTile;

class Character
{
    friend class CharacterFactory;

    friend class CharacterInfo;

    friend class CharacterModel;

    struct Ability 
    {
        Integer Score;

        Integer Modifier;

        Ability() {}

        Ability(Integer score) : Score(score) 
        {
            Modifier = (Score - 10) / 2;
        }
    };

    Group* group;

    CharacterInfo* info;

    CharacterTypes type;

    Integer level;

    Integer hitDiceCount;

    Integer hitPointCount;

    Ability strength;

    Ability dexterity;

    Ability constitution;

    Ability intelligence;

    Ability wisdom;

    Ability charisma;

    Ability* primaryAbility;

    Integer armorClass;

    Integer proficiencyBonus;

    BattleTile* tile;

    Character();

public:
    void Attack(Character&);
    
    void Select();

    void Deselect();

    Position2 GetPosition();
};