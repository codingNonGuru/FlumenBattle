#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenBattle/BattleMap.h"

Array <BattleTile*> nearbyTiles;

BattleMap::BattleMap(Length size) 
{
    nearbyTiles.Initialize(size * size);

    auto height = (int)((float)size * 0.866f);
    tiles.Initialize(size, height);

    for(Integer i = 0; i < tiles.GetWidth(); ++i)
    {
        for(Integer j = 0; j < tiles.GetHeight(); ++j)
        {
            auto x = float (i) * 0.866f;
            auto y = float(j);

            if(i % 2 == 1)
            {
                y += 0.5f;
            }

            *tiles.Get(i, j) = {Position2(x, y), Integer2(i, j)};
        }
    }
}

BattleTile* GetHex(Integer x, Integer y, Integer z)
{
    return nullptr;
}

BattleTile* BattleMap::GetRandomTile()
{
    auto i = utility::GetRandom(0, tiles.GetWidth());
    auto j = utility::GetRandom(0, tiles.GetHeight());

    return tiles.Get(i, j);
}

BattleTile* BattleMap::GetCenterTile()
{
    return tiles.Get(tiles.GetWidth() / 2, tiles.GetHeight() / 2);
}