#include "FlumenCore/Utility/Utility.hpp"

#include "Earthquake.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/WorldMap.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/Settlement/Settlement.h"

using namespace world::disaster;

Earthquake::Earthquake(WorldTile *tile, int strength)
{
    this->epicenter = tile;
    this->strength = strength;
}

void Earthquake::ApplyEffect() const
{
    auto &nearbyTiles = epicenter->GetNearbyTiles(10);
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
    auto strength = 0;

    auto diceRoll = utility::GetRandom(1, 20);
    if(diceRoll == 20)
    {
        strength = 2;
    }
    else if(diceRoll > 15)
    {
        strength = 1;
    }

    auto tile = WorldScene::Get()->GetWorldMap()->GetRandomLandTile();

    const auto earthquake = Earthquake(tile, strength);
    earthquake.ApplyEffect();
}