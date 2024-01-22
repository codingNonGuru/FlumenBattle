#include "FlumenEngine/Utility/Color.hpp"

#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/WorldMap.h"
#include "FlumenBattle/World/WorldBiome.h"
#include "FlumenBattle/World/WorldRelief.h"
#include "FlumenBattle/World/Settlement/Path.h"

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
    auto seaColor = Color(0.02f, 0.1f, 0.35f, 1.0f);
    auto rockColor = Color(0.25f, 0.25f, 0.25f, 1.0f);
    auto peakColor = Color(0.95f, 0.95f, 0.95f, 1.0f);

    auto color = [&] 
    {
        if(Type == WorldTiles::SEA)
        {
            auto depthFactor = (float)Elevation / 50.0f;
            depthFactor *= depthFactor * depthFactor;
            depthFactor *= depthFactor * depthFactor;
            return seaColor * (1.0f - depthFactor) + Color::CYAN * depthFactor;
        }

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

    auto variationFactor = VARIATION_FACTOR;
    if(Type == WorldTiles::SEA)
        variationFactor *= 0.3f;

    color = Color::AddSaturation(color, utility::GetRandom(-variationFactor, variationFactor));
    color = Color::Lighten(color, utility::GetRandom(-variationFactor, variationFactor));

    Shade = color;
}

const Array <WorldTile*> & WorldTile::GetNearbyTiles(Integer range, int bufferIndex)
{
    return map->GetNearbyTiles(this, range, bufferIndex);
}

const container::Block <WorldTile *, 6> WorldTile::GetNearbyTiles()
{
    return map->GetNearbyTiles(this);
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

    auto &nearbyTiles = GetNearbyTiles(2);
    for(auto tile = nearbyTiles.GetStart(); tile != nearbyTiles.GetEnd(); ++tile)
    {
        (*tile)->isBorderingOwnedTile = true;
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