#pragma once

#include "FlumenCore/Conventions.hpp"

namespace battle
{
    struct BattleTile;

    class BattleMap
    {
        friend class BattleScene;

        friend class HumanController;

        friend class BattleTileModel;

        Grid <BattleTile> tiles;

        BattleMap() {}

        BattleMap(Length size);

        BattleTile* GetRandomTile();

    public:
        static constexpr Float TILE_DISTANCING = 60.0f; 

        static constexpr auto TILE_SIZE = [] {return (TILE_DISTANCING / 2.0f) / 0.8666f;} ();

        BattleTile* GetTile(Integer3);

        BattleTile* GetEmptyRandomTile();

        BattleTile* GetCenterTile();

        BattleTile* GetEmptyTileAroundTile(BattleTile *, Integer);

        const Array<BattleTile *> & GetNearbyTiles(BattleTile*, Integer);

        container::Block <BattleTile *, 6> GetNearbyTiles(BattleTile*);

        const Array<BattleTile *> &GetTileRing(BattleTile *, Integer);

        const Grid <BattleTile> &GetTiles() const {return tiles;}
    };
}