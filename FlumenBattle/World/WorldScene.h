#pragma once

#include "FlumenEngine/Core/Scene.hpp"

#include "FlumenBattle/World/WorldTime.h"

class Delegate;

namespace world::settlement
{
    class Settlement;
    struct Path;
    struct PathSegment;
}

namespace world
{
    class WorldMap;
    class WorldTile;

    namespace polity
    {
        class Polity;
        class Faction;
    }
    
    namespace group
    {
        class Group;
        class Encounter;
        struct GroupBuffer;
    }

    namespace render
    {
        class WorldTileModel;
    }

    class WorldScene : Scene
    {
        friend class WorldState;

        friend class WorldController;

        friend class render::WorldTileModel;

        friend class WorldGenerator;

        friend class WorldAllocator;

        friend class GroupAllocator;

        WorldTime time;

        WorldMap *worldMap;

        Pool <group::Encounter> *battles;

        group::Group *playerGroup;

        Pool <group::Group> *groups;

        Pool <settlement::Settlement> *settlements;

        Pool <polity::Polity> *polities;

        Pool <settlement::Path> *paths;

        Pool <settlement::PathSegment> *pathSegments;

        Array <WorldTile *> ownershipChangeQueue;

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

        Delegate *OnSettlementFounded;

        static WorldScene * Get() 
        {
            static WorldScene scene;

            return &scene;
        }

        group::Group * GetPlayerGroup() const {return playerGroup;}

        WorldMap * GetWorldMap() const {return worldMap;}

        container::Grid <WorldTile> &GetTiles() const;

        Pool <settlement::Settlement> & GetSettlements() {return *settlements;}

        Pool <settlement::Path> & GetPaths() {return *paths;}

        Pool <group::Encounter> & GetBattles() {return *battles;}

        Pool <group::Group> & GetGroups() {return *groups;}

        Pool <polity::Polity> & GetPolities() {return *polities;}

        const WorldTime & GetTime() const {return time;}

        const settlement::Settlement *GetFoundedSettlement() const;

        bool IsTimeFlowing() const {return time;}

        int GetTimeSpeed() const {return time;}

        group::Group *GetGroup(int);

        const group::GroupBuffer GetNearbyGroups(WorldTile *, int = 1);

        void InitiateEncounter(group::Group *, group::Group *);

        void InitiatePlayerPersuasion();

        void InitiatePlayerBattle();

        void FinishPlayerBattle();

        void FinishPlayerEncounter();

        settlement::Settlement * FoundSettlement(WorldTile *, settlement::Settlement *);

        settlement::Settlement * ForgePath(settlement::Settlement *, settlement::Settlement *, int = INT_MAX);

        polity::Polity *FoundPolity(settlement::Settlement *);

        polity::Polity *SplitPolity(polity::Faction *);

        void UpdateOwnershipChangeQueue(WorldTile *);

        const Array <WorldTile *> &GetOwnershipChangeQueue() const {return ownershipChangeQueue;}

        bool IsNightTime() const {return time.HourCount < 6 || time.HourCount >= 22;}
    };
}