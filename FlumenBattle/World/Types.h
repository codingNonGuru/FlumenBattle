#pragma once

namespace world
{
    namespace group
    {
        enum class GroupTypes {PLAYER, COMPUTER};

        enum class GroupActions
        {
            TAKE_SHORT_REST, TAKE_LONG_REST, SEARCH, ENGAGE, FIGHT, TRAVEL, NONE
        };

        enum class SuccessTypes
        {
            CRITICAL_SUCCESS, SUCCESS, FAILURE, CRITICAL_FAILURE
        };

        enum class ActionIntensities
        {
            LEISURELY, NORMAL, INTENSE
        };
    }

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

    enum class SettlementProductionOptions
    {
        NONE,
        PATROL,
        FARM,
        SETTLERS,
        MONUMENT,
        WALLS,
        GRANARY,
        SEWAGE,
        IRRIGATION
    };

    enum class AfflictionResultTypes
    {
        NONE,
        CURE,
        REGRESSION,
        PROGRESSION,
        DEATH
    };

    enum class SettlementEventTypes
    {
        DEPOPULATION
    };
}