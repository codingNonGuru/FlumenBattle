#pragma once

namespace world
{
    namespace group
    {
        enum class GroupTypes {PLAYER, COMPUTER};

        enum class GroupClasses {MERCHANT, BANDIT, ADVENTURER, TRAVELLING_COURT};

        enum class GroupActions
        {
            TAKE_SHORT_REST, TAKE_LONG_REST, SEARCH, ENGAGE, FIGHT, DISENGAGE, TRAVEL, PERSUADE, NONE
        };

        enum class ActionIntensities
        {
            LEISURELY, NORMAL, INTENSE
        };

        enum class Attitudes
        {
            FRIENDLY, INDIFFERENT, HOSTILE
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
        IRRIGATION,
        LIBRARY
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
}