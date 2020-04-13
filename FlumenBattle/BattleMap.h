#pragma once

#include "FlumenCore/Conventions.hpp"

struct BattleTile;

class BattleMap
{
    friend class BattleScene;

    friend class BattleTileModel;

    Grid <BattleTile> tiles;

    BattleMap() {}

    BattleMap(Length size);

    BattleTile* GetRandomTile();

public:
    static const Float TILE_DISTANCING = 60.0f;

    BattleTile* GetTile(Integer3);

    BattleTile* GetEmptyRandomTile();

    BattleTile* GetCenterTile();

    BattleTile* GetEmptyTileAroundTile(BattleTile *, Integer);

    const Array<BattleTile*> & GetNearbyTiles(BattleTile*, Integer);
};