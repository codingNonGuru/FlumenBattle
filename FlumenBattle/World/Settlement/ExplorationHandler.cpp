#include "ExplorationHandler.h"
#include "FlumenBattle/Utility/Utility.h"

using namespace world::settlement;

ExplorationReward ExplorationHandler::GetReward(WorldTile *tile)
{
    auto diceRoll = 10;//utility::RollD100Dice();
    if(diceRoll >= 1 && diceRoll <= 5)
    {
        return {ExplorationRewards::POPULATION};
    }
    else if(diceRoll <= 20)
    {
        auto techIndex = utility::GetRandom(0, (int)science::Technologies::COUNT - 1);

        return {ExplorationRewards::TECH_BOOST, {science::Technologies(techIndex)}};
    }
    else
    {
        return {ExplorationRewards::RESOURCE_CACHE, ExplorationReward::ResourceBatch{ResourceTypes::POTTERY, 20}};
    }
}