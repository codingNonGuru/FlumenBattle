#include "Earthquake.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Tile/WorldMap.h"
#include "FlumenBattle/World/Tile/WorldTile.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/Utility/Utility.h"

using namespace world::disaster;

Earthquake::Earthquake(tile::WorldTile *tile, int strength)
{
    this->epicenter = tile;
    this->strength = strength;
}

void Earthquake::ApplyEffect() const
{
    auto nearbyTiles = epicenter->GetNearbyTiles(10);
    for(auto &tile : nearbyTiles)
    {
        auto settlement = tile->GetSettlement();
        if(settlement == nullptr)
            continue;

        settlement->ProcessEarthquake(*this);
    }
}

void EarthquakeGenerator::Generate()
{
    using namespace utility;

    auto strength = 0;

    auto diceRoll = RollD20Dice(DifficultyClass(15));
    if(diceRoll.IsCriticalSuccess())
    {
        strength = 2;
    }
    else if(diceRoll.IsNormalSuccess())
    {
        strength = 1;
    }

    auto tile = WorldScene::Get()->GetWorldMap()->GetRandomLandTile();

    const auto earthquake = Earthquake(tile, strength);
    earthquake.ApplyEffect();
}