#include "FlumenEngine/Utility/Color.hpp"

#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/WorldMap.h"
#include "FlumenBattle/World/WorldBiome.h"
#include "FlumenBattle/World/WorldRelief.h"
#include "FlumenBattle/World/Settlement/Path.h"
#include "FlumenBattle/World/WorldScene.h"

using namespace world;

static constexpr auto VARIATION_FACTOR = 0.07f;

const auto SEASONAL_TEMPERATURE_SWING = 17.0f;

const auto DIURNAL_TEMPERATURE_SWING = 1.5f;

const auto WINTER_THRESHOLD = 50;

WorldTile::WorldTile(Position2 position, Integer2 squareCoordinates) : Position(position), SquareCoordinates(squareCoordinates), 
group(nullptr), settlement(nullptr), owner(nullptr), isBorderingOwnedTile(false)
{
    HexCoordinates.x = squareCoordinates.x - squareCoordinates.y / 2;
    HexCoordinates.z = squareCoordinates.y;
    HexCoordinates.y = -HexCoordinates.x - HexCoordinates.z;
}

void WorldTile::Initialize()
{
    auto const DIRT_COLOR = Color(0.9f, 0.7f, 0.5f, 1.0f);
    auto const GRASS_COLOR = Color(0.4f, 0.6f, 0.05f, 1.0f);
    auto const SEA_COLOR = Color(0.02f, 0.07f, 0.3f, 1.0f);
    auto const ROCK_COLOR = Color(0.25f, 0.25f, 0.25f, 1.0f);
    auto const PEAK_COLOR = Color(0.95f, 0.95f, 0.95f, 1.0f);

    auto color = [&] 
    {
        if(Type == WorldTiles::SEA)
        {
            auto depthFactor = (float)Elevation / 50.0f;
            depthFactor *= depthFactor * depthFactor;
            depthFactor *= depthFactor * depthFactor;
            return SEA_COLOR * (1.0f - depthFactor) + Color::CYAN * depthFactor;
        }

        if(HasRelief(WorldReliefs::MOUNTAINS))
        {
            auto count = 0;
            auto nearbyTiles = map->GetNearbyTiles(this, 1);
            for(auto &tile : nearbyTiles)
            {
                if(tile->HasRelief(WorldReliefs::MOUNTAINS) == false)
                    return ROCK_COLOR;
            }

            return PEAK_COLOR;
        }
        
        switch(Biome->Type)
        {
            case WorldBiomes::DESERT:
                return DIRT_COLOR;
            case WorldBiomes::STEPPE:
                return DIRT_COLOR * 0.3f + GRASS_COLOR * 0.7f;
            case WorldBiomes::WOODS:
                return GRASS_COLOR * 0.9f;
        }
    } ();

    auto variationFactor = VARIATION_FACTOR;
    if(Type == WorldTiles::SEA)
        variationFactor *= 0.3f;

    color = Color::AddSaturation(color, utility::GetRandom(-variationFactor, variationFactor));
    color = Color::Lighten(color, utility::GetRandom(-variationFactor, variationFactor));

    Shade = color;
}

const TileBuffer WorldTile::GetNearbyTiles(Integer range)
{
    return map->GetNearbyTiles(this, range);
}

const container::Block <WorldTile *, 6> WorldTile::GetNearbyTiles()
{
    return map->GetNearbyTiles(this);
}

const TileBuffer WorldTile::GetTileRing(Integer range)
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

/*Integer WorldTile::GetTravelPenalty() const
{
    return Biome->TravelPenalty + Relief->TravelPenalty;
}*/

int WorldTile::GetPenalty() const
{
    if(Type == WorldTiles::SEA)
    {
        return 10;
    }

    if(Relief->Type == WorldReliefs::MOUNTAINS)
    {
        return 5;
    }

    if(Biome->Type == WorldBiomes::WOODS)
    {
        return 3;
    }

    return 1;
}

TravelPenalty WorldTile::GetTravelPenalty() const
{
    container::SmartBlock <TravelPenaltyTypes, 4> penalties;

    if(Biome->Type == WorldBiomes::WOODS)
    {
        *penalties.Add() = TravelPenaltyTypes::WOODS; 
    }

    if(Relief->Type == WorldReliefs::MOUNTAINS)
    {
        *penalties.Add() = TravelPenaltyTypes::MOUNTAINS; 
    }

    if(Type == WorldTiles::SEA)
    {
        *penalties.Add() = TravelPenaltyTypes::SEA; 
    }

    return {Biome->TravelPenalty + Relief->TravelPenalty, penalties};
}

bool WorldTile::HasBiome(WorldBiomes biome) const 
{
    return biome == Biome->Type;
}

bool WorldTile::HasRelief(WorldReliefs relief) const 
{
    return relief == Relief->Type;
}

void WorldTile::Settle(settlement::Settlement *settlement)
{
    this->settlement = settlement;
}

void WorldTile::AssertOwnership(settlement::Settlement *owner)
{
    this->owner = owner;

    auto nearbyTiles = GetNearbyTiles(2);
    for(auto &tile : nearbyTiles)
    {
        tile->isBorderingOwnedTile = true;
    }
}

bool WorldTile::IsLinkedTo(WorldTile *tile) 
{
    for(auto iterator = links.GetFirst(); iterator != links.GetLast(); ++iterator)
    {
        if(**iterator == settlement::PathSegment(this, tile))
        {
            return true;
        }
    }
    
    return false;
}

void WorldTile::AddLink(WorldTile *tile, settlement::PathSegment *segment) 
{
    *links.Add() = segment;
}

settlement::PathSegment * WorldTile::GetLinkTo(WorldTile *tile)
{
    for(auto iterator = links.GetFirst(); iterator != links.GetLast(); ++iterator)
    {
        if(**iterator == settlement::PathSegment(this, tile))
            return *iterator;
    }
    
    return nullptr;
}

int WorldTile::GetSeasonalTemperature() const
{
    auto swing = WorldTile::GetSeasonalTemperatureSwing();

    return Heat + int(swing);
}

bool WorldTile::IsWinter() const
{
    return GetSeasonalTemperature() < WINTER_THRESHOLD;
}

int WorldTile::GetActualTemperature() const
{
    return Heat + int(WorldTile::GetSeasonalTemperatureSwing() + WorldTile::GetDiurnalTemperatureSwing());
}

float WorldTile::GetSeasonalTemperatureSwing() 
{
    static auto &worldTime = WorldScene::Get()->GetTime();

    auto timeFactor = float(worldTime.TotalMinuteCount) / float(WorldTime::MINUTES_IN_YEAR);
    timeFactor *= TWO_PI;

    auto swing = -cos(timeFactor) * SEASONAL_TEMPERATURE_SWING;

    return swing;
}

float WorldTile::GetDiurnalTemperatureSwing() 
{
    static auto &worldTime = WorldScene::Get()->GetTime();

    auto timeFactor = float(worldTime.TotalMinuteCount) / float(WorldTime::MINUTES_IN_DAYS);
    timeFactor *= TWO_PI;
    timeFactor += HALF_PI;

    auto swing = -cos(timeFactor) * DIURNAL_TEMPERATURE_SWING;

    return swing;
}