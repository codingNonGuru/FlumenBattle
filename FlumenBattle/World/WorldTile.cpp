#include "FlumenEngine/Utility/Color.hpp"

#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/WorldMap.h"
#include "FlumenBattle/World/WorldBiome.h"
#include "FlumenBattle/World/WorldRelief.h"

using namespace world;

static constexpr auto VARIATION_FACTOR = 0.07f;

WorldTile::WorldTile(Position2 position, Integer2 squareCoordinates) : Position(position), SquareCoordinates(squareCoordinates), 
group(nullptr), settlement(nullptr), owner(nullptr), isBorderingOwnedTile(false)
{
    HexCoordinates.x = squareCoordinates.x - squareCoordinates.y / 2;
    HexCoordinates.z = squareCoordinates.y;
    HexCoordinates.y = -HexCoordinates.x - HexCoordinates.z;
}

void WorldTile::Initialize()
{
    auto dirtColor = Color(0.9f, 0.7f, 0.5f, 1.0f);
    auto grassColor = Color(0.4f, 0.6f, 0.05f, 1.0f);
    auto seaColor = Color(0.05f, 0.3f, 0.6f, 1.0f);
    auto rockColor = Color(0.25f, 0.25f, 0.25f, 1.0f);
    auto peakColor = Color(0.95f, 0.95f, 0.95f, 1.0f);

    auto color = [&] {
        if(Type == WorldTiles::SEA)
            return seaColor;

        if(HasRelief(WorldReliefs::MOUNTAINS))
        {
            auto count = 0;
            auto &nearbyTiles = map->GetNearbyTiles(this, 1);
            for(auto tile = nearbyTiles.GetStart(); tile != nearbyTiles.GetEnd(); ++tile)
            {
                if((*tile)->HasRelief(WorldReliefs::MOUNTAINS) == false)
                    return rockColor;
            }

            return peakColor;
        }
        
        switch(Biome->Type)
        {
            case WorldBiomes::DESERT:
                return dirtColor;
            case WorldBiomes::STEPPE:
                return dirtColor * 0.3f + grassColor * 0.7f;
            case WorldBiomes::WOODS:
                return grassColor * 0.9f;
        }
    } ();

    color = Color::AddSaturation(color, utility::GetRandom(-VARIATION_FACTOR, VARIATION_FACTOR));
    color = Color::Lighten(color, utility::GetRandom(-VARIATION_FACTOR, VARIATION_FACTOR));

    Shade = color;
}

const Array <WorldTile*> & WorldTile::GetNearbyTiles(Integer range)
{
    return map->GetNearbyTiles(this, range);
}

const Array <WorldTile*> & WorldTile::GetTileRing(Integer range)
{
    return map->GetTileRing(this, range);
}

WorldTile* WorldTile::GetEmptyTileInRange(Integer range)
{
    return map->GetEmptyTileAroundTile(this, range);
}

WorldTile * WorldTile::GetNeighbor(Integer3 direction)
{
    auto coordinates = HexCoordinates + direction;
    return map->GetTile(coordinates);
}

Float4 WorldTile::GetShade() const
{
    return Shade;
}

bool WorldTile::HasBiome(WorldBiomes biome) const 
{
    return biome == Biome->Type;
}

bool WorldTile::HasRelief(WorldReliefs relief) const 
{
    return relief == Relief->Type;
}

void WorldTile::Settle(Settlement *settlement)
{
    this->settlement = settlement;
}

void WorldTile::AssertOwnership(Settlement *owner)
{
    this->owner = owner;

    auto &nearbyTiles = GetNearbyTiles(1);
    for(auto tile = nearbyTiles.GetStart(); tile != nearbyTiles.GetEnd(); ++tile)
    {
        (*tile)->isBorderingOwnedTile = true;
    }
}