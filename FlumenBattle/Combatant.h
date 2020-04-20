#pragma once

#include "FlumenCore/Conventions.hpp"

class Character;
class CharacterInfo;
struct BattleTile;
struct CharacterActionData;
class CombatGroup;
class Condition;

class Combatant
{
    friend class container::Array <Combatant>;

    friend class Character; 

    friend class CombatGroup;

    friend class BattleController;

    friend class ArtificialController;

    friend class HumanController;

    friend class BattleScene;

    friend class CharacterInfo;

    friend class BattleInterface;

    friend class BattleTileModel;

    friend class Condition;

    friend class SpellCaster;

    CombatGroup *group;

    Character *character;

    CharacterInfo *info; 

    Integer movement; 

    Integer speedPenalty;

    Integer armorClass; 

    Integer remainingActionCount; 

    Combatant *target; 

    bool isSavingAgainstDeath; 

    Integer deathThrowSuccesCount; 

    Integer deathThrowFailureCount; 

    BattleTile *tile; 

    Pool <Condition> conditions;

    Combatant();

    void Initialize(CombatGroup *, Character *, BattleTile *);

    Position2 GetPosition();

    Integer GetMovement() const;

    Integer GetDistanceTo(Combatant *) const;

    Integer GetCurrentSpeed() const;

    void StartTurn();

    Integer RollInitiative() const;

    bool HasAdvantage() const;

    bool HasDisadvantage() const;

    bool CanMove() const;

    bool CanTarget() const;

    bool CanAct(Combatant *);

    bool CanStrike() const;

    bool CanCastSpell() const;

    bool CanDodge() const;

    bool CanDash() const;

    CharacterActionData Act(Combatant *);

    CharacterActionData Strike();

    CharacterActionData CastSpell();

    CharacterActionData Dodge();

    CharacterActionData Dash();

    bool Move(BattleTile *);

    bool SufferDamage(Integer);

    bool HealDamage(Integer);

    void SaveAgainstDeath();

    void AddCondition(Condition);
        
    void Select();

    void Deselect();

    Integer RollAttackDamage(bool) const;

    bool IsDodging();

public:
    CombatGroup * GetGroup() const;

    Character * GetCharacter() const {return character;}

    bool IsAlive() const;

    Combatant * GetTarget() const {return target;}

    bool IsSavingAgainstDeath() const {return isSavingAgainstDeath;}

    Float GetHealth() const;

    BattleTile *GetTile() const {return tile;}
};