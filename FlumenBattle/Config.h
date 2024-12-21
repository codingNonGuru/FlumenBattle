#pragma once

#include "FlumenEngine/Core/ConfigManager.h"

namespace game
{
    struct ConfigValues
    {
        static constexpr auto MAXIMUM_WORLD_SIZE = "MAXIMUM_WORLD_SIZE";
        static constexpr auto MINIMUM_WORLD_SIZE = "MINIMUM_WORLD_SIZE";
        static constexpr auto DEFAULT_WORLD_SIZE = "DEFAULT_WORLD_SIZE";
        static constexpr auto SIMULATION_CHANGE_DELAY = "SIMULATION_CHANGE_DELAY";
        static constexpr auto MAXIMUM_CHARACTERS_PER_GROUP = "MAXIMUM_CHARACTERS_PER_GROUP";
        static constexpr auto WINTER_SURVIVAL_DC = "WINTER_SURVIVAL_DC";
        static constexpr auto WINTER_FROSTBITE_DC = "WINTER_FROSTBITE_DC";
        static constexpr auto GROUP_SPOTTING_LIMIT = "GROUP_SPOTTING_LIMIT";
        static constexpr auto MAXIMUM_SPOTTING_LIFETIME = "MAXIMUM_SPOTTING_LIFETIME";
        static constexpr auto GROUP_SEARCH_BASE_DC = "GROUP_SEARCH_BASE_DC";
        static constexpr auto GROUP_BATCH_TILE_SIZE = "GROUP_BATCH_TILE_SIZE";
        static constexpr auto GROUPS_PER_BATCH = "GROUPS_PER_BATCH";
        static constexpr auto NIGHT_PERCEPTION_PENALTY = "NIGHT_PERCEPTION_PENALTY";
        static constexpr auto TERRAIN_PERCEPTION_PENALTY = "TERRAIN_PERCEPTION_PENALTY";
        static constexpr auto MAXIMUM_GROUP_SPOTTING_DISTANCE = "MAXIMUM_GROUP_SPOTTING_DISTANCE";
        static constexpr auto FATIGUE_ONSET_SINCE_REST = "FATIGUE_ONSET_SINCE_REST";
        static constexpr auto MAXIMUM_QUEST_COUNT = "MAXIMUM_QUEST_COUNT";
        static constexpr auto DELIVER_ITEM_QUEST_BASE_DURATION = "DELIVER_ITEM_QUEST_BASE_DURATION";
        static constexpr auto FLANKING_ARMOR_PENALTY = "FLANKING_ARMOR_PENALTY";
        static constexpr auto MAXIMUM_POLITY_SIZE = "MAXIMUM_POLITY_SIZE";
        static constexpr auto MAXIMUM_WALLS_LEVEL = "MAXIMUM_WALLS_LEVEL";
        static constexpr auto VOLUME_PER_RESOURCE_UNIT = "VOLUME_PER_RESOURCE_UNIT";
        static constexpr auto TICKS_PER_NEED_SATISFACTION = "TICKS_PER_NEED_SATISFACTION";
        static constexpr auto NEED_SATISFACTION_THRESHOLD = "NEED_SATISFACTION_THRESHOLD";
        static constexpr auto TICKS_PER_HAPPINESS = "TICKS_PER_HAPPINESS";
        static constexpr auto CONTENTEDNESS_THRESHOLD = "CONTENTEDNESS_THRESHOLD";
        static constexpr auto HAPPINESS_THRESHOLD = "HAPPINESS_THRESHOLD";
        static constexpr auto ECSTASY_THRESHOLD = "ECSTASY_THRESHOLD";
        static constexpr auto MAXIMUM_PATHS_PER_SETTLEMENT = "MAXIMUM_PATHS_PER_SETTLEMENT";
        static constexpr auto BORDER_EXPANSION_MAX_DISTANCE = "BORDER_EXPANSION_MAX_DISTANCE";
        static constexpr auto EXPLORATIONS_PER_SETTLEMENT = "EXPLORATIONS_PER_SETTLEMENT";
    };
}