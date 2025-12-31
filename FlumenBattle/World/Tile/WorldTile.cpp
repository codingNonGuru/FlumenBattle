#include "FlumenEngine/Utility/Color.hpp"

#include "FlumenBattle/World/Tile/WorldTile.h"
#include "FlumenBattle/World/Tile/WorldMap.h"
#include "FlumenBattle/World/Tile/WorldBiome.h"
#include "FlumenBattle/World/Tile/WorldRelief.h"
#include "FlumenBattle/World/Settlement/Path.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/SettlementTile.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/WorldAllocator.h"
#include "FlumenBattle/World/Tile/RuinHandler.h"

using namespace world::tile;

static constexpr auto VARIATION_FACTOR = 0.07f;

const auto SEASONAL_TEMPERATURE_SWING = 17.0f;

const auto DIURNAL_TEMPERATURE_SWING = 1.5f;

const auto WINTER_THRESHOLD = 50;

WorldTile::WorldTile(Position2 position, Integer2 squareCoordinates) : SquareCoordinates(squareCoordinates), 
settlement(nullptr), owner(nullptr), isBorderingOwnedTile(false)
{
    Position = position;

    Coordinates.x = squareCoordinates.x - squareCoordinates.y / 2;
    Coordinates.z = squareCoordinates.y;
    Coordinates.y = -Coordinates.x - Coordinates.z;

    isRevealed = true;

    ruinHandler = nullptr;
}

void WorldTile::Initialize()
{
    IsPeak = [&] 
    {
        if(HasRelief(WorldReliefs::MOUNTAINS) == true)
        {
            auto count = 0;
            auto nearbyTiles = map->GetNearbyTiles(this, 1);
            for(auto &tile : nearbyTiles)
            {
                if(tile->HasRelief(WorldReliefs::MOUNTAINS) == false)
                    return false;
            }

            return true;
        }
    } ();

    auto const DESERT_COLOR = Color(0.9f, 0.5f, 0.2f, 1.0f);
    auto const DIRT_COLOR = Color(0.9f, 0.7f, 0.5f, 1.0f);
    auto const GRASS_COLOR = Color(0.4f, 0.6f, 0.05f, 1.0f);
    auto const SEA_COLOR = Color(0.02f, 0.07f, 0.3f, 1.0f);
    auto const ROCK_COLOR = Color(0.25f, 0.25f, 0.25f, 1.0f);
    auto const PEAK_COLOR = Color(0.95f, 0.95f, 0.95f, 1.0f);

    auto color = [&] 
    {
        if(Type == WorldTiles::SEA)
        {
            auto depthFactor = (float)Elevation / 500.0f;
            depthFactor *= depthFactor * depthFactor;
            depthFactor *= depthFactor * depthFactor;
            return SEA_COLOR * (1.0f - depthFactor) + Color::CYAN * depthFactor;
        }
        
        switch(Biome->Type)
        {
            case WorldBiomes::DESERT:
                if(IsArid() == true)
                    return DESERT_COLOR;
                else
                    return DIRT_COLOR;
            case WorldBiomes::STEPPE:
                if(IsScrubland == true)
                {
                    return DIRT_COLOR * 0.7f + GRASS_COLOR * 0.3f;
                }
                else
                {
                    return DIRT_COLOR * 0.3f + GRASS_COLOR * 0.7f;
                }
            case WorldBiomes::WOODS:
                return DIRT_COLOR * 0.3f + GRASS_COLOR * 0.7f;
                //return GRASS_COLOR * 0.87f;
        }
    } ();

    auto variationFactor = VARIATION_FACTOR;
    if(Type == WorldTiles::SEA)
        variationFactor *= 0.3f;

    color = Color::AddSaturation(color, utility::GetRandom(-variationFactor, variationFactor));
    color = Color::Lighten(color, utility::GetRandom(-variationFactor, variationFactor));

    Shade = color;
}

bool WorldTile::IsArid() const
{
    return Biome->Type == WorldBiomes::DESERT && Heat >= 86;
}

const engine::thread::HexBuffer <WorldTile> WorldTile::GetNearbyTiles(Integer range)
{
    return map->GetNearbyTiles(this, range);
}

container::Block <WorldTile *, 6> WorldTile::GetNearbyTiles()
{
    return map->GetNearbyTiles(this);
}

const engine::thread::HexBuffer <WorldTile> WorldTile::GetTileRing(Integer range)
{
    return map->GetTileRing(this, range);
}

WorldTile* WorldTile::GetEmptyTileInRange(Integer range)
{
    return map->GetEmptyTileAroundTile(this, range);
}

WorldTile * WorldTile::GetNeighbor(Integer3 direction)
{
    auto coordinates = Coordinates + direction;
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

    WorldScene::Get()->UpdateOwnershipChangeQueue(this);
}

void WorldTile::RemoveSettlement() 
{
    for(auto &tile : settlement->GetTiles())
    {
        tile.Tile->owner = nullptr;

        WorldScene::Get()->UpdateOwnershipChangeQueue(tile.Tile);
    }

    for(auto &tile : settlement->GetTiles())
    {
        bool hasOwnedNeighbor = false;
        for(auto &neighbor : tile.Tile->GetNearbyTiles(2).Tiles)
        {
            if(neighbor->owner != nullptr)
            {
                hasOwnedNeighbor = true;
                break;
            }
        }

        if(hasOwnedNeighbor == false)
        {
            tile.Tile->isBorderingOwnedTile = false;
        }
    }

    settlement = nullptr;
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

world::settlement::PathSegment * WorldTile::GetLinkTo(WorldTile *tile)
{
    for(auto iterator = links.GetFirst(); iterator != links.GetLast(); ++iterator)
    {
        if(**iterator == settlement::PathSegment(this, tile))
            return *iterator;
    }
    
    return nullptr;
}

void WorldTile::AddRuin(settlement::Settlement *settlement, bool isSettlementCenter)
{
    if(ruinHandler == nullptr)
    {
        ruinHandler = WorldAllocator::Get()->AllocateRuinHandler();
    }

    ruinHandler->Add(settlement, isSettlementCenter);
}

int WorldTile::GetRuinCount() const
{
    if(ruinHandler == nullptr)
    {
        return 0;
    }
    else
    {
        return ruinHandler->GetCount();
    }
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