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