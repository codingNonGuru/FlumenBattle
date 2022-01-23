#include "FlumenEngine/Utility/Color.hpp"

#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/WorldMap.h"
#include "FlumenBattle/World/WorldBiome.h"

using namespace world;

WorldTile::WorldTile(Position2 position, Integer2 squareCoordinates) : Position(position), SquareCoordinates(squareCoordinates), group(nullptr)
{
    HexCoordinates.x = squareCoordinates.x - squareCoordinates.y / 2;
    HexCoordinates.z = squareCoordinates.y;
    HexCoordinates.y = -HexCoordinates.x - HexCoordinates.z;
}

void WorldTile::Initialize()
{
    if(map->GetCenterTile()->GetPhysicalDistanceTo(*this) > 700.0f)
    {
        Type = WorldTiles::SEA;
    }
    else
    {
        Type = WorldTiles::LAND;
        if(utility::GetRandom(1, 100) < 33)
        {
            Biome = WorldBiomeFactory::BuildBiome(WorldBiomes::DESERT);
        }
        else if(utility::GetRandom(1, 100) < 50)
        {
            Biome = WorldBiomeFactory::BuildBiome(WorldBiomes::STEPPE);
        }
        else
        {
            Biome = WorldBiomeFactory::BuildBiome(WorldBiomes::WOODS);
        }
    }

    auto dirtColor = Color(0.9f, 0.7f, 0.5f, 1.0f);
    auto grassColor = Color(0.4f, 0.6f, 0.05f, 1.0f);
    auto seaColor = Color(0.05f, 0.3f, 0.6f, 1.0f);

    auto color = [&] {
        if(Type == WorldTiles::SEA)
            return seaColor;
        
        switch(Biome->Type)
        {
            case WorldBiomes::DESERT:
                return dirtColor;
            case WorldBiomes::STEPPE:
                return dirtColor * 0.5f + grassColor * 0.5f;
            case WorldBiomes::WOODS:
                return grassColor;
        }
    } ();

    color = Color::AddSaturation(color, utility::GetRandom(-0.03f, 0.03f));
    color = Color::Lighten(color, utility::GetRandom(-0.03f, 0.03f));

    Shade = color;
}

const Array <WorldTile*> & WorldTile::GetNearbyTiles(Integer range)
{
    return map->GetNearbyTiles(this, range);
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