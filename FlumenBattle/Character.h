#pragma once

#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/CharacterActionData.h"

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

    CharacterClasses type;

    Integer level;

    Integer hitDiceCount;

    Integer currentHitPoints;

    Integer maximumHitPoints;

    Ability strength;

    Ability dexterity;

    Ability constitution;

    Ability intelligence;

    Ability wisdom;

    Ability charisma;

    Ability* primaryAbility;

    Integer armorClass;

    Integer proficiencyBonus;

    Integer defaultSpeed;

    Integer movement;

    Integer defaultRange;

    Integer actionCount;

    BattleTile* tile;

    Character();

    void Initialize();

    bool CanMove() const;

    bool CanAttack() const;

    bool IsAlive() const;

    Integer GetSpeed() const;

    bool SufferDamage(Integer);

    bool HasAdvantage() const;

    bool HasDisadvantage() const;

public:
    void StartTurn();

    bool CanAttackTarget(const Character &);

    CharacterActionData Attack(Character&);

    void Move(BattleTile*);
    
    void Select();

    void Deselect();

    Position2 GetPosition();

    Integer GetMovement() const;

    BattleTile* GetTile() const {return tile;}

    Integer RollInitiative() const;

    Word GetName();
};