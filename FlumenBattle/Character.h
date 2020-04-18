#pragma once

#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/CharacterActionData.h"
#include "FlumenBattle/Proficiency.h"
#include "FlumenBattle/ProficiencyHandler.h"

class CharacterFactory;
class CharacterInfo;
class Group;
class BattleTile;
struct Weapon;
struct Spell;
struct SpellSlot;
class Condition;
class ProficiencyHandler;

class Character
{
public:
    struct Action
    {
        CharacterActions Type;

        Action() : Type() {}

        Action(CharacterActions type) : Type(type) {}

        bool operator== (const Action &other) {return this->Type == other.Type;}
    };

private:
    friend class container::Array<Character>;

    friend class CharacterFactory;

    friend class CharacterInfo;

    friend class BattleInterface;

    friend class Condition; 

    friend class SpellCaster;

    friend class ArtificialController;

    friend class HumanController;

    friend class ProficiencyHandler;

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

    Group *group;

    CharacterInfo *info;

    CharacterClasses type;

    Integer level;

    ProficiencyHandler proficiencies;

    Integer hitDiceCount;

    Integer currentHitPoints;

    Integer maximumHitPoints;

    Ability strength;

    Ability dexterity;

    Ability constitution;

    Ability intelligence;

    Ability wisdom;

    Ability charisma;

    Ability *attackAbility;

    Ability *spellCastingAbility;

    Integer armorClass;

    Integer defaultSpeed;

    Integer speedPenalty;

    Integer movement;

    Integer remainingActionCount;

    Array <Action> actions;

    Action *selectedAction;

    Array <Weapon> weapons;

    Weapon *selectedWeapon;

    Array <Spell> spells;

    Spell *selectedSpell;

    Array <SpellSlot> spellSlots;

    Character *target;

    bool isSavingAgainstDeath;

    Integer deathThrowSuccesCount;

    Integer deathThrowFailureCount;

    Pool <Condition> conditions;

    BattleTile* tile;

    Character();

    void Initialize();

    bool CanMove() const;

    bool CanStrike() const;

    bool CanCastSpell() const;

    bool CanDodge() const;

    bool CanDash() const;

    CharacterActionData Attack();

    CharacterActionData CastSpell();

    CharacterActionData Dodge();

    CharacterActionData Dash();

    Integer GetCurrentSpeed() const;

    Integer GetActionRange() const;

    bool SufferDamage(Integer);

    bool HealDamage(Integer);

    bool HasAdvantage() const;

    bool HasDisadvantage() const;

    void AddWeapon(Weapon);

    void AddSpell(Spell);

    void AddCondition(Condition);

    void AddProficiency(Proficiency);

    Integer RollAttackDamage() const;

    bool IsDodging();

public:
    bool IsAlive() const;

    Float GetHealth() const {return (Float)currentHitPoints / (Float)maximumHitPoints;}

    void StartTurn();

    bool CanTarget() const;

    bool CanAct(Character *);

    CharacterActionData Act(Character *);

    bool Move(BattleTile*);

    void SaveAgainstDeath();
    
    void Select();

    void Deselect();

    Position2 GetPosition();

    Integer GetMovement() const;

    BattleTile* GetTile() const {return tile;}

    Integer GetDistanceTo(Character *) const;

    Integer RollInitiative() const;

    Word GetName();

    Group * GetGroup() const {return group;}

    Integer GetMagicProficiencyBonus() {return proficiencies.GetMagicBonus(*this);}

    Integer GetReflexSaveBonus();

    Integer GetFortitudeSaveBonus();

    Integer GetWillSaveBonus();

    Array <Action> & GetActions() {return actions;}

    const Array <Weapon> & GetWeapons() {return weapons;}

    const Array <Spell> & GetSpells() {return spells;}

    bool SelectAction(Index);

    bool SelectActionOption(Index);

    bool SelectAction(CharacterActions);

    bool SelectWeapon(Weapon *);

    bool SelectWeapon(WeaponTypes);

    bool SelectSpell(Spell *);

    bool SelectSpell(SpellTypes);

    Index GetSelectedActionIndex() const;

    Index GetSelectedSubactionIndex() const;

    Action* GetSelectedAction() const;

    Weapon* GetSelectedWeapon() const {return selectedWeapon;}

    Spell* GetSelectedSpell() const {return selectedSpell;}

    Character* GetTarget() const {return target;}

    bool IsSavingAgainstDeath() const {return isSavingAgainstDeath;}
};