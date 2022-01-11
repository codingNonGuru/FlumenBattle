#include "FlumenEngine/Utility/Color.hpp"

#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/WorldMap.h"

using namespace world;

WorldTile::WorldTile(Position2 position, Integer2 squareCoordinates) : Position(position), SquareCoordinates(squareCoordinates)
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

    auto color = utility::GetRandom(1, 100) < 33 ? 
        dirtColor : 
        utility::GetRandom(1, 100) < 50 ? grassColor : dirtColor * 0.5f + grassColor * 0.5f;

    if(Map->GetCenterTile()->GetDistanceTo(*this) > 12)
    {
        color = seaColor;
    }

    color = Color::AddSaturation(color, utility::GetRandom(-0.07f, 0.07f));
    color = Color::Lighten(color, utility::GetRandom(-0.07f, 0.07f));

    Shade = color;
}

const Array <WorldTile*> & WorldTile::GetNearbyTiles(Integer range)
{
    return Map->GetNearbyTiles(this, range);
}

WorldTile* WorldTile::GetEmptyTileInRange(Integer range)
{
    return Map->GetEmptyTileAroundTile(this, range);
}

WorldTile * WorldTile::GetNeighbor(Integer3 direction)
{
    auto coordinates = HexCoordinates + direction;
    return Map->GetTile(coordinates);
}