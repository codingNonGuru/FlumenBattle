#pragma once

#define MINIMUM_COLONIZATION_RANGE 5

#define MAXIMUM_COLONIZATION_RANGE 6

namespace world::settlement
{
    enum class ProductionOptions
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

    enum class BuildingTypes
    {
        SEWAGE,
        WALLS,
        GRANARY,
        MONUMENT,
        IRRIGATION,
        LIBRARY
    };

    enum class AbundanceLevels
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
        SAVING_THROWS_AGAINST_MALARIA,
        FACTION_JOIN_INTERDICTION
    };

    enum class RoadTypes
    {
        UNTRODDEN,
        TRODDEN,
        WELL_TRODDEN,
        PAVED,
        WELL_PAVED
    };

    #define GOODS_TYPES_COUNT 5

    #define BASIC_RESOURCES_COUNT 5

    enum class ResourceTypes
    {
        FOOD,
        TIMBER,
        METAL,
        WOOL,
        STONE
        //LEATHER
        //POTTERY
        //FURNITURE
        //TOOLS
        //CARTS
        //PAPER
        //WINE
        //ROPE
        //GLASS
        //SOAP
        //CLOTHING
        //SHOES
        //BOATS
        //MACHINERY
        //JEWELRY
        //SPICE
        //INCENSE
        //TEA
        //MULES
        //CANDLES
        //MEDICINE
        //CUTLERY
        //FURS
        //GOLD
        //DYES
        //TOBACCO
        //IVORY
    };
}