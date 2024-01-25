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
    };
}