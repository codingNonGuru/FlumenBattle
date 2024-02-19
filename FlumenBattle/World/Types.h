#pragma once

namespace world
{
    enum class WorldTiles
    {
        LAND, SEA
    };

    enum class WorldBiomes
    {
        STEPPE, DESERT, WOODS, SWAMP, MARINE
    };

    enum class WorldClimates
    {
        ARCTIC, TEMPERATE, TROPICAL
    };

    enum class WorldReliefs
    {
        PLAINS, HILLS, MOUNTAINS, SEA
    };

    enum class AfflictionTypes
    {
        MALARIA,
        TUBERCULOSIS,
        HUNGER
    };

    enum class AfflictionResultTypes
    {
        NONE,
        CURE,
        REGRESSION,
        PROGRESSION,
        DEATH
    };

    namespace settlement
    {
        enum class EventTypes
        {
            DEPOPULATION
        };

        enum class Conditions
        {
            SICKENED,
            IMMUNITY_AGAINST_MALARIA,
            REPRESSED
        };
    }

    enum class SimulationLevels
    {
        BASIC,
        MEDIUM,
        ADVANCED
    };
}