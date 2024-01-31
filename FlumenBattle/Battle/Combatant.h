#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenBattle/World/Character/Types.h"

namespace world::character
{
    class Character;
    class Condition;
}

enum class SpellTypes;

namespace battle
{
    class CombatGroup;
    struct BattleTile;
    class CharacterInfo;
    struct CharacterActionData;

    class Combatant
    {
        friend class container::Array <Combatant>;

        friend class world::character::Character; 

        friend class CombatGroup;

        friend class BattleController;

        friend class ArtificialController;

        friend class HumanController;

        friend class BattleScene;

        friend class CharacterInfo;

        friend class BattleInterface;

        friend class BattleTileModel;

        friend class world::character::Condition;

        friend class SpellCaster;

        CombatGroup *group;

        world::character::Character *character;

        CharacterInfo *info; 

        Integer movement; 

        Integer speedPenalty;

        Integer armorClass; 

        Integer remainingActionCount; 

        Integer bonusActionCount; 

        Combatant *target; 

        bool isSavingAgainstDeath; 

        Integer deathThrowSuccesCount; 

        Integer deathThrowFailureCount; 

        BattleTile *tile;

        Position2 position; 

        float rotation;

        //Pool <world::character::Condition> conditions;

        Combatant();

        void Initialize(CombatGroup *, world::character::Character *, BattleTile *);

        Integer GetDistanceTo(Combatant *) const;

        Integer GetCurrentSpeed() const;

        void StartTurn();

        Integer RollInitiative() const;

        bool HasAdvantage() const;

        bool HasDisadvantage() const;

        bool CanMove(int) const;

        bool CanTarget() const;

        bool CanAct(Combatant *);

        bool CanStrike() const;

        bool CanCastSpell() const;

        bool HasSlot(SpellTypes);

        bool CanDodge() const;

        bool CanDash() const;

        CharacterActionData Act(Combatant *);

        CharacterActionData Strike();

        CharacterActionData CastSpell();

        CharacterActionData Dodge();

        CharacterActionData Dash();

        void SetTile(BattleTile *);

        bool Move(BattleTile *, int);

        bool SufferDamage(Integer);

        bool HealDamage(Integer);

        void SaveAgainstDeath();

        void AddCondition(world::character::Conditions, world::character::Character *);
            
        void Select();

        void Deselect();

        bool IsDodging();

    public:
        CombatGroup * GetGroup() const;

        world::character::Character * GetCharacter() const {return character;}

        bool IsAlive() const;

        Combatant * GetTarget() const {return target;}

        bool IsSavingAgainstDeath() const {return isSavingAgainstDeath;}

        Float GetHealth() const;

        BattleTile *GetTile() const {return tile;}

        Position2 GetPosition() const;

        float GetRotation() const;

        const Integer &GetRemainingActionCount() const {return remainingActionCount;}

        const Integer &GetMovement() const;

        bool IsWithinActionRange(BattleTile *);

        void SetPosition(Position2 newPosition) {position = newPosition;}

        void SetRotation(float newRotation) {rotation = newRotation;}
    };
}