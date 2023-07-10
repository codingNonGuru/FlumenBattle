#pragma once

#include "FlumenEngine/Core/Scene.hpp"

#include "FlumenBattle/World/WorldTime.h"

class Delegate;

namespace world::settlement
{
    class Settlement;
}

namespace world
{
    class WorldMap;
    class WorldTile;
    class Polity;
    namespace group
    {
        class Group;
        class Encounter;
    }

    class WorldScene : Scene
    {
        friend class WorldState;

        friend class WorldController;

        friend class WorldTileModel;

        friend class WorldGenerator;

        friend class WorldAllocator;

        friend class GroupAllocator;

        WorldTime time;

        WorldMap *worldMap;

        Pool <group::Encounter> *battles;

        group::Group *playerGroup;

        Pool <group::Group> *groups;

        Pool <settlement::Settlement> *settlements;

        Pool <Polity> polities;

        WorldScene();

        void Initialize() override;

        void Update() override;

        void Render() override;

        void HandleEnable() override;

        void HandleDisable() override;

        void Refresh();

        void StartTime(int delay = 0) {time.StartTime(delay);}

        void StopTime(int delay = 0) {time.StopTime(delay);}

        void ToggleTime() {if(time) time.StopTime(); else time.StartTime();}

        void SpeedUpTime();

        void SlowDownTime();

        void HandleBattleRoundEnded();

    public:
        Delegate *OnUpdateStarted;

        Delegate *OnPlayerEncounterInitiated;

        Delegate *OnPlayerEncounterFinished;

        Delegate *OnPlayerBattleStarted;

        Delegate *OnPlayerBattleEnded;

        static WorldScene * Get() 
        {
            static WorldScene scene;

            return &scene;
        }

        group::Group * GetPlayerGroup() const {return playerGroup;}

        WorldMap * GetWorldMap() const {return worldMap;}

        Pool <settlement::Settlement> & GetSettlements() {return *settlements;}

        Pool <group::Encounter> & GetBattles() {return *battles;}

        Pool <group::Group> & GetGroups() {return *groups;}

        const WorldTime & GetTime() const {return time;}

        bool IsTimeFlowing() const {return time;}

        int GetTimeSpeed() const {return time;}

        void InitiateEncounter(group::Group *, group::Group *);

        void InitiatePlayerPersuasion();

        void InitiatePlayerBattle();

        void FinishPlayerBattle();

        void FinishPlayerEncounter();

        settlement::Settlement * FoundSettlement(WorldTile *, Polity *);
    };
}