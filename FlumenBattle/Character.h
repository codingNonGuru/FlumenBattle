#pragma once

#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/CharacterActionData.h"

class CharacterFactory;
class CharacterInfo;
class Group;
class BattleTile;
struct Weapon;
struct Spell;

class Character
{
public:
    struct Action
    {
        CharacterActions Type;
    };

private:
    friend class container::Array<Character>;

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

    Ability* attackAbility;

    Ability* spellCastingAbility;

    Integer armorClass;

    Integer proficiencyBonus;

    Integer defaultSpeed;

    Integer movement;

    Integer remainingActionCount;

    Array <Action> actions;

    Action * selectedAction;

    Array <Weapon> weapons;

    Weapon * selectedWeapon;

    Array <Spell> spells;

    Spell * selectedSpell;

    bool isSavingAgainstDeath;

    Integer deathThrowSuccesCount;

    Integer deathThrowFailureCount;

    BattleTile* tile;

    Character();

    void Initialize();

    bool CanMove() const;

    bool CanAttack() const;

    bool CanCastSpell() const;

    Integer GetSpeed() const;

    Integer GetActionRange() const;

    bool SufferDamage(Integer);

    bool HasAdvantage() const;

    bool HasDisadvantage() const;

    void AddWeapon(Weapon);

    void AddSpell(Spell);

    Integer RollAttackDamage() const;

public:
    bool IsAlive() const;

    void StartTurn();

    bool CanAct(const Character &);

    bool CanAttackTarget(const Character &);

    bool CanCastSpellAgainstTarget(const Character &);

    CharacterActionData Act(Character&);

    CharacterActionData Attack(Character&);

    CharacterActionData CastSpell(Character&);

    void Move(BattleTile*);

    void SaveAgainstDeath();
    
    void Select();

    void Deselect();

    Position2 GetPosition();

    Integer GetMovement() const;

    BattleTile* GetTile() const {return tile;}

    Integer RollInitiative() const;

    Word GetName();

    Array <Action> & GetActions() {return actions;}

    const Array <Weapon> & GetWeapons() {return weapons;}

    const Array <Spell> & GetSpells() {return spells;}

    bool SelectAction(Index);

    bool SelectActionOption(Index);

    Action* GetSelectedAction() const;

    Weapon* GetSelectedWeapon() const {return selectedWeapon;}

    Spell* GetSelectedSpell() const {return selectedSpell;}

    bool IsSavingAgainstDeath() const {return isSavingAgainstDeath;}
};