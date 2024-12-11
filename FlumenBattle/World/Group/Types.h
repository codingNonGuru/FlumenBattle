#pragma once

namespace world::group
{
    constexpr int TILES_PER_GROUP_ROUTE = 8;

    constexpr int TILES_PER_PLAYER_ROUTE = 32;

    enum class GroupClasses 
    {
        PLAYER, MERCHANT, BANDIT, ADVENTURER, TRAVELLING_COURT, PATROL, GARRISON, RAIDER
    };

    enum class GroupTypes {PLAYER, COMPUTER};

    enum class GroupActions
    {
        TAKE_SHORT_REST, TAKE_LONG_REST, SEARCH, ENGAGE, FIGHT, DISENGAGE, TRAVEL, PERSUADE, FORAGE, BYPASS_DEFENCES, NONE
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