#pragma once

namespace world::group
{
    constexpr int TILES_PER_GROUP_ROUTE = 8;

    constexpr int TILES_PER_PLAYER_ROUTE = 32;

    enum class GroupClasses 
    {
        PLAYER, MERCHANT, BANDIT, ADVENTURER, TRAVELLING_COURT, PATROL
    };
}