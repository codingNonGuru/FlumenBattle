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
}