#include "Path.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/Settlement/SettlementAllocator.h"

using namespace world::settlement;

void Path::AddTile(WorldTile *tile) 
{
    *Tiles.Add() = tile;

    if(Tiles.GetSize() == 1)
        return;

    auto previousTile = *(Tiles.GetLast() - 2);

    if(tile->IsLinkedTo(previousTile) == true)
        return;

    auto segment = settlement::SettlementAllocator::Get()->AllocateSegment();

    *segment = {tile, previousTile, settlement::RoadTypes::TRODDEN}; 

    previousTile->AddLink(tile, segment);

    tile->AddLink(previousTile, segment);
}

const container::SmartBlock <world::WorldTile *, 16> Path::GetTilesTo(const Settlement *settlement) const
{
    container::SmartBlock <world::WorldTile *, 16> tiles;

    bool isForward = true;

    auto firstTile = *Tiles.Get(0);
    if(firstTile->GetSettlement() == settlement)
    {
        isForward = false;
    }

    auto i = isForward ? 0 : Tiles.GetSize() - 1;
    while(true)
    {
        *tiles.Add() = *Tiles.Get(i);

        i += isForward ? 1 : -1;
        bool willBreak = isForward ? i == Tiles.GetSize() : i == -1;
        if(willBreak)
        {
            break;
        }
    }

    return tiles;
}