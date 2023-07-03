#pragma once

#include "FlumenBattle/World/Types.h"

namespace world
{
    struct WorldRelief
    {
        WorldReliefs Type;

        int TravelPenalty;
    };

    class WorldReliefFactory
    {
    public:
        static const WorldRelief * BuildRelief(WorldReliefs type)
        {
            switch(type)
            {
                case WorldReliefs::PLAINS:
                    return BuildPlainsRelief();
                case WorldReliefs::HILLS:
                    return BuildHillsRelief();
                case WorldReliefs::MOUNTAINS:
                    return BuildMountainsRelief();
                case WorldReliefs::SEA:
                    return BuildSeaRelief();
            }
        }

    private:
        static const WorldRelief * BuildPlainsRelief()
        {
            static WorldRelief relief = {WorldReliefs::PLAINS, 0};
            return &relief;
        }

        static const WorldRelief * BuildHillsRelief()
        {
            static WorldRelief relief = {WorldReliefs::HILLS, 2};
            return &relief;
        }

        static const WorldRelief * BuildMountainsRelief()
        {
            static WorldRelief relief = {WorldReliefs::MOUNTAINS, 4};
            return &relief;
        }

        static const WorldRelief * BuildSeaRelief()
        {
            static WorldRelief relief = {WorldReliefs::SEA, 0};
            return &relief;
        }
    };
}