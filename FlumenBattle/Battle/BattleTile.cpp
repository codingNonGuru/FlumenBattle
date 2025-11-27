#include "FlumenEngine/Utility/Color.hpp"

#include "FlumenBattle/Battle/BattleTile.h"
#include "FlumenBattle/Battle/BattleMap.h"
#include "FlumenBattle/Types.hpp"

using namespace battle;

BattleTile::BattleTile(Position2 position, Integer2 squareCoordinates) : SquareCoordinates(squareCoordinates), Combatant(nullptr)
{
    Position = position;

    Coordinates.x = squareCoordinates.x - squareCoordinates.y / 2;
    Coordinates.z = squareCoordinates.y;
    Coordinates.y = -Coordinates.x - Coordinates.z;

    IsObstacle = utility::GetRandom(1, 100) < 15 ? true : false;

    Color color(0.8f, 0.6f, 0.5f, 1.0f);
    color = Color::AddSaturation(color, utility::GetRandom(-0.1f, 0.1f));
    color = Color::Lighten(color, utility::GetRandom(-0.1f, 0.1f));

    if(IsObstacle)
    {
        color = Color::Lighten(color, -0.5f);
    }

    Shade = color;
}

TravelPenalty BattleTile::GetTravelPenalty() const
{
    container::SmartBlock <TravelPenaltyTypes, 4> penalties;

    if(IsObstacle)
    {
        *penalties.Add() = TravelPenaltyTypes::MOUNTAINS;
    }

    return {1, penalties};
}

int BattleTile::GetPenalty() const
{
    return IsObstacle ? 3 : 1;
}

const engine::thread::HexBuffer <BattleTile> BattleTile::GetNearbyTiles(Integer range)
{
    return Map->GetNearbyTiles(this, range);
}

container::Block <BattleTile *, 6> BattleTile::GetNearbyTiles()
{
    return Map->GetNearbyTiles(this);
}

const engine::thread::HexBuffer <BattleTile> BattleTile::GetTileRing(Integer range)
{
    return Map->GetTileRing(this, range);
}   

BattleTile* BattleTile::GetEmptyTileInRange(Integer range)
{
    return Map->GetEmptyTileAroundTile(this, range);
}

BattleTile * BattleTile::GetNeighbor(Integer3 direction)
{
    auto coordinates = Coordinates + direction;
    return Map->GetTile(coordinates);
}

auto visitedTiles = Array <BattleTile::OldPathData>(1024);

auto fringeTiles = [] () {
    static auto arrays = Array <Array <BattleTile *>>(128);

    for(int i = 0; i < arrays.GetCapacity(); ++i)
    {
        *arrays.Add() = Array <BattleTile *> (128);
    }

    return arrays;
} ();

const Array <BattleTile::OldPathData> & BattleTile::FindPath(Integer range)
{
    visitedTiles.Reset();
    *visitedTiles.Add() = {this, 0};

    fringeTiles.Reset();
    
    auto tileArray = fringeTiles.Add();
    tileArray->Reset();
    *tileArray->Add() = this;

    for(int k = 1; k <= range; k++)
    {
        fringeTiles.Add()->Reset();
        for(auto &tile : *fringeTiles.Get(k - 1))
        {
            auto neighbours = tile->GetNearbyTiles(1);
            for(auto neighbour : neighbours)
            {
                if(neighbour->IsObstacle || neighbour->Combatant)
                    continue;

                bool hasFound = false;
                for(auto &visitedTile : visitedTiles)
                {
                    if(visitedTile.Tile == neighbour)
                    {
                        hasFound = true;
                        break;
                    }
                }

                if(hasFound == true)
                    continue;

                *visitedTiles.Add() = {neighbour, k};
                *fringeTiles.Get(k)->Add() = neighbour;
            }
        }
    }

    return visitedTiles;
}