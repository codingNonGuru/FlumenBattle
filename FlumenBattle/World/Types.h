#pragma once

namespace world
{
    enum class GroupTypes {PLAYER, COMPUTER};

    enum class GroupActions
    {
        TAKE_SHORT_REST, TAKE_LONG_REST, SEARCH, FIGHT, TRAVEL, NONE
    };

    enum class WorldTiles
    {
        LAND, SEA
    };

    enum class WorldBiomes
    {
        STEPPE, DESERT, WOODS, SWAMP
    };

    enum class WorldClimates
    {
        ARCTIC, TEMPERATE, TROPICAL
    };

    enum class WorldReliefs
    {
        PLAINS, HILLS, MOUNTAINS
    };
}