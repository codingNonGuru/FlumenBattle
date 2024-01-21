#pragma once

#include "FlumenCore/Delegate/Delegate.hpp"

#include "FlumenBattle/Battle/CharacterActionData.h"

namespace world::character
{
    class Character;
}

class Group;

namespace battle
{
    class Combatant;
    struct BattleTile;
    class BattleScene;
    
    class BattleController
    {
        friend class BattleState;

        friend class ActionInfoPanel;

        friend class ArtificialController;

        friend class HumanController;

        BattleScene *battleScene;

        BattleTile *targetedTile;

        Combatant *selectedCombatant;

        Combatant *targetedCombatant;

        CharacterActionData lastActionData;

        bool isPlayerInputEnabled;

        static BattleController *instance;

        void Initialize();

        void HandleSceneEnabled();

        void DetermineCharacterController();

        void Move();

        void CheckTileSelection();

        void SelectAction(Integer);

        void SelectSubaction(Integer);

        void Act();

        void EndTurn();

    public:
        Delegate OnCharacterActed;

        Delegate OnCharacterSelected;

        Delegate OnActionSelected;

        Delegate OnSubactionSelected;

        Delegate OnBattleEnded;

        void SelectCombatant(Combatant *);

        void TargetCombatant(Combatant *);

        void TargetTile(BattleTile * tile) {targetedTile = tile;}

        BattleTile * GetTargetedTile() const {return targetedTile;}

        Combatant * GetSelectedCombatant() const {return selectedCombatant;}

        Combatant * GetTargetedCombatant() const {return targetedCombatant;}

        world::character::Character * GetSelectedCharacter() const;

        world::character::Character * GetTargetedCharacter() const;

        const CharacterActionData & GetLastAction() const {return lastActionData;}

        void ExitBattle();

        static BattleController * Get();
    };
}