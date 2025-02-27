#pragma once

#define MINIMUM_COLONIZATION_RANGE 5

#define MAXIMUM_COLONIZATION_RANGE 6

namespace world::settlement
{
    enum class ProductionOptions
    {
        PATROL,
        GARRISON,
        FARM,
        SETTLERS,
        MONUMENT,
        WALLS,
        GRANARY,
        SEWAGE,
        IRRIGATION,
        LIBRARY,
        HOUSING,
        LUMBER_MILL,
        CARPENTER,
        BAKERY,
        MARKET,
        WEAVING_MILL,
        TAILORY,
        KEEP,
        POTTERY,
        NONE
    };

    enum class ProductionClasses {RECRUITMENT, BUILDING, NONE};

    enum class BuildingTypes
    {
        SEWAGE,
        WALLS,
        GRANARY,
        MONUMENT,
        IRRIGATION,
        LIBRARY,
        HOUSING,
        LUMBER_MILL,
        CARPENTER,
        BAKERY,
        MARKET,
        WEAVING_MILL,
        TAILORY,
        KEEP,
        POTTERY
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
        FACTION_JOIN_INTERDICTION,
        PRODUCTION_FROM_METAL_RESOURCE,
        FOOD_PRODUCTION_FROM_WOOD_TILES,
        COMBAT_BONUS_ON_WOOD_TILES,
        COMBAT_BONUS_ON_MOUNTAIN_TILES,
        DURATION_BETWEEN_TRADE_SHIPMENTS,
        POPULATION_GROWTH_RATE,
        DISTANCE_TO_CAPITAL_INDEPENDENCE_LIMIT,
        WOOD_RELATED_RESOURCE_PRODUCTION,
        DEFENDER_GROUP_BONUS_AC,
        DEFENDER_GROUP_BONUS_INITIATIVE,
        LOOT_DC_BONUS,
        PILLAGE_DC_BONUS
    };

    enum class RoadTypes
    {
        UNTRODDEN,
        TRODDEN,
        WELL_TRODDEN,
        PAVED,
        WELL_PAVED
    };

    #define GOODS_TYPES_COUNT 16

    #define BASIC_RESOURCES_COUNT 5

    enum class ResourceTypes
    {
        FOOD, //MEAT, GRAIN, FRUIT, MUSHROOM, SEAFOOD, HERBS
        TIMBER,
        METAL,
        FIBER,
        CLAY,
        LUMBER,
        FURNITURE,
        COOKED_FOOD,
        FABRIC,
        CLOTHING,
        POTTERY,
        NONE
    };

    enum class SettlementAttitudes
    {
        HOSTILE,
        UNFRIENDLY,
        COLD,
        INDIFFERENT,        
        OPEN,
        WELCOMING,
        FRIENDLY
    };

    enum class TileImprovements
    {   
        FARM,
        PASTURE,
        CLAY_PIT,
        MINE,
        SAW_MILL,
        NONE
    };

    enum class ExplorationRewards
    {
        RESOURCE_CACHE,
        RESOURCE_SOURCE,
        TECH_BOOST,
        SPELL,
        POPULATION,
        RELIC,
        CORNERSTONE,
        BLESSING,
        ITEM,
        CURSE,
        DISEASE,
        INVADER,
        EPIC_QUEST,
        EPIC_ALLY_GROUP,
        DERELICT_IMPROVEMENT,
        NATURAL_DISASTER
    };
}