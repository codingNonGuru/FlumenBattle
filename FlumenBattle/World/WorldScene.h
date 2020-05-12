#pragma once

#include "FlumenEngine/Core/Scene.hpp"

#include "FlumenBattle/World/WorldTime.h"

class Delegate;

namespace world
{
    class Battle;
    namespace group
    {
        class Group;
    }

    class WorldScene : Scene
    {
        friend class WorldState;

        friend class WorldController;

        WorldTime time;

        Pool <Battle> battles;

        group::Group *playerGroup;

        Pool <group::Group> groups;

        WorldScene();

        void Initialize() override;

        void Update() override;

        void Render() override;

        void HandleEnable() override;

        void HandleDisable() override;

        void Refresh();

        void StartTime() {time = true;}

        void StopTime() {time = false;}

        void ToggleTime() {time = time ? false : true;}

        void SpeedUpTime();

        void SlowDownTime();

        void HandleBattleRoundEnded();

    public:
        Delegate *OnUpdateStarted;

        static WorldScene * Get() 
        {
            static WorldScene scene;

            return &scene;
        }

        group::Group * GetPlayerGroup() const {return playerGroup;}

        Pool <Battle> & GetBattles() {return battles;}

        Pool <group::Group> & GetGroups() {return groups;}

        const WorldTime & GetTime() const {return time;}

        bool IsTimeFlowing() const {return time;}

        int GetTimeSpeed() const {return time;}

        void StartBattle(group::Group *, group::Group *);
    };
}