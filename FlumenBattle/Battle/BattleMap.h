#pragma once

#include "FlumenCore/Conventions.hpp"
#include "FlumenCore/Hex/Hex.h"

#include "FlumenEngine/Container/HexGrid.h"
#include "FlumenEngine/Thread/HexBuffer.h"

#include "FlumenBattle/Battle/BattleTile.h"

namespace battle
{
    class BattleMap : public container::HexGrid <BattleTile>
    {
        friend class BattleScene;

        friend class HumanController;

        friend class BattleTileModel;

        BattleMap() {}

        BattleMap(Length size);

    public:
        static constexpr Float TILE_DISTANCING = 60.0f; 

        static constexpr auto TILE_SIZE = [] {return (TILE_DISTANCING / 2.0f) / 0.8666f;} ();

        BattleTile* GetEmptyRandomTile();

        BattleTile* GetEmptyTileAroundTile(BattleTile *, Integer);
    };
}