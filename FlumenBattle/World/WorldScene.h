#pragma once

#include "FlumenEngine/Core/Scene.hpp"

class Delegate;

namespace world
{
    class Battle;
    namespace group
    {
        class Group;
    }

    struct WorldTime
    {
        int HourCount;

        int DayCount;

        int YearCount;

        WorldTime() {}

        WorldTime(int yearCount, int dayCount, int hourCount) : YearCount(yearCount), DayCount(dayCount), HourCount(hourCount) {}

        WorldTime& operator++()
        {
            HourCount++;
            if(HourCount >= 24)
            {
                HourCount = 0;

                DayCount++;
                if(DayCount >= 365)
                {
                    DayCount = 0;
                    YearCount++;
                }
            }

            return *this;
        }
    };

    class WorldScene : Scene
    {
        friend class WorldState;

        friend class WorldController;

        bool isTimeFlowing;

        int timeSpeed;

        WorldTime time;

        //Battle *playerBattle;

        Pool <Battle> battles;

        group::Group *playerGroup;

        Array <group::Group> groups;

        WorldScene();

        void Initialize() override;

        void Update() override;

        void Render() override;

        void HandleEnable() override;

        void HandleDisable() override;

        void StartTime() {isTimeFlowing = true;}

        void StopTime() {isTimeFlowing = false;}

        void ToggleTime() {isTimeFlowing = isTimeFlowing ? false : true;}

        void SpeedUpTime();

        void SlowDownTime();

    public:
        Delegate *OnUpdateStarted;

        static WorldScene * Get() 
        {
            static WorldScene scene;

            return &scene;
        }

        //Battle * GetPlayerBattle() const {return playerBattle;}

        group::Group * GetPlayerGroup() const {return playerGroup;}

        Pool <Battle> & GetBattles() {return battles;}

        Array <group::Group> & GetGroups() {return groups;}

        const WorldTime & GetTime() const {return time;}

        bool IsTimeFlowing() const {return isTimeFlowing;}

        int GetTimeSpeed() const {return timeSpeed;}

        void StartBattle(group::Group *, group::Group *);
    };
}