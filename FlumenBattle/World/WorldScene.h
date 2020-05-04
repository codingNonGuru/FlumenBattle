#pragma once

#include "FlumenEngine/Core/Scene.hpp"

class WorldController;

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

        friend class ::WorldController;

        bool isTimeFlowing;

        float timeFlowRate;

        int timeSpeed;

        Integer hourCount;

        Battle *battle;

        group::Group *playerGroup;

        Array <group::Group> groups;

        void Initialize() override;

        void Update() override;

        void Render() override;

        void HandleEnable() override;

        void HandleDisable() override;

        void StartBattle();

        void StartTime() {isTimeFlowing = true;}

        void StopTime() {isTimeFlowing = false;}

        void ToggleTime() {isTimeFlowing = isTimeFlowing ? false : true;}

        void SpeedUpTime();

        void SlowDownTime();

    public:
        static WorldScene * Get() 
        {
            static WorldScene scene;

            return &scene;
        }

        Battle * GetBattle() const {return battle;}

        group::Group * GetPlayerGroup() const {return playerGroup;}

        Integer GetHourCount() const {return hourCount;}
    };
}