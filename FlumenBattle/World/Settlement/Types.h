#pragma once

namespace world::settlement
{
    enum class BuildingTypes
    {
        SEWAGE,
        WALLS,
        GRANARY,
        MONUMENT,
        IRRIGATION,
        LIBRARY
    };

    enum class FoodSecurity
    {
        CORNUCOPIA,
        ABUNDANT,
        ENOUGH,
        BARELY_AVAILABLE,
        LACKING,
        SORELY_LACKING
    };

    enum class Modifiers
    {
        BUILDING_SAVING_THROWS_AGAINST_EARTHQUAKES,
        SAVING_THROWS_AGAINST_DISEASE,
        PATROL_ATTACK_ROLLS,
        FOOD_PRODUCTION_ON_DESERT_TILES,
        SCIENCE_PRODUCTION,
        ALL_DICE_ROLLS,
        MALARIA_EMERGENCE_DIFFICULTY,
        SAVING_THROWS_AGAINST_MALARIA
    };
}