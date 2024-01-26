#pragma once

#include "FlumenEngine/Core/ConfigManager.h"

namespace game
{
    struct ConfigValues
    {
        static constexpr auto DEFAULT_WORLD_SIZE = "DEFAULT_WORLD_SIZE";
        static constexpr auto SIMULATION_CHANGE_DELAY = "SIMULATION_CHANGE_DELAY";
        static constexpr auto WINTER_SURVIVAL_DC = "WINTER_SURVIVAL_DC";
        static constexpr auto WINTER_FROSTBITE_DC = "WINTER_FROSTBITE_DC";
        static constexpr auto GROUP_SPOTTING_LIMIT = "GROUP_SPOTTING_LIMIT";
        static constexpr auto MAXIMUM_SPOTTING_LIFETIME = "MAXIMUM_SPOTTING_LIFETIME";
        static constexpr auto GROUP_SEARCH_BASE_DC = "GROUP_SEARCH_BASE_DC";
        static constexpr auto GROUP_BATCH_TILE_SIZE = "GROUP_BATCH_TILE_SIZE";
        static constexpr auto GROUPS_PER_BATCH = "GROUPS_PER_BATCH";
    };
}