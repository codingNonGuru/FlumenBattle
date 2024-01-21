#pragma once

#include "FlumenCore/Delegate/Delegate.hpp"

#include "FlumenEngine/Core/Scene.hpp"
#include "FlumenCore/Singleton.h"

#include "FlumenBattle/Battle/CharacterActionData.h"

class CharacterInfo;

namespace battle
{
    class Combatant;
    class CombatGroup;
    class BattleTileModel;
    class BattleMap;
    class BattleTile;
    class BattleInterface;

    struct Turn
    {
        class Combatant * Combatant;

        Integer Initiative;

        bool operator > (const Turn &other)
        {
            return Initiative > other.Initiative;
        }
    };

    class BattleScene : public Scene, public core::Singleton <BattleScene>
    {
        friend class BattleInterface;

        friend class BattleTileModel;

        friend class BattleState;

        friend class BattleController;

        BattleTileModel *battleTileModel;

        BattleMap *battleMap;

        CombatGroup *playerGroup;

        CombatGroup *computerGroup;

        Array <Turn> turnOrder;

        Turn *turn;

        bool isGrabbingScreen {false};

        Position2 grabStartPosition;

        Position2 grabPositionDelta;

        void Initialize() override;

        void HandleEnable() override;

        void HandleDisable() override;

        void Render() override;

        void Update() override;

        void DetermineTurnOrder();

        void HandleGrabPressed();

        void HandleGrabReleased();

        void HandleCombatantActed();

    public:
        Delegate OnInitialized;

        Delegate OnEnabled;

        Delegate OnRoundEnded;

        BattleScene();

        BattleMap* GetBattleMap() {return battleMap;}

        Turn* GetTurn() {return turn;}

        Combatant * GetActingCharacter() {return turn->Combatant;}

        Camera * GetCamera() const;

        CombatGroup * GetPlayerGroup() const {return playerGroup;}

        CombatGroup * GetComputerGroup() const {return computerGroup;}

        bool IsCharactersTurn(Combatant *) const;

        void EndTurn();

        //static BattleScene* Get(); 
    };
}