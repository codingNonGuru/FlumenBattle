#pragma once

#include "FlumenCore/Delegate/Delegate.hpp"

#include "FlumenEngine/Core/Scene.hpp"
#include "FlumenEngine/Core/Singleton.h"

#include "FlumenBattle/CharacterActionData.h"

class BattleInterface;
class CharacterInfo;
class Combatant;
class CombatGroup;
class BattleTileModel;
class BattleMap;
class BattleTile;

struct Turn
{
    class Combatant * Combatant;

    Integer Initiative;

    bool operator > (const Turn &other)
    {
        return Initiative > other.Initiative;
    }
};

class BattleScene : public Scene, public Singleton<BattleScene>
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

    void Initialize() override;

    void HandleEnable() override;

    void HandleDisable() override;

    void Render() override;

    void Update() override;

    void DetermineTurnOrder();

public:
    Delegate OnInitialized;

    Delegate OnEnabled;

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