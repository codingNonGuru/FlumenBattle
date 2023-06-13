#include "WorldGenerator.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/WorldMap.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/WorldBiome.h"
#include "FlumenBattle/World/WorldRelief.h"

using namespace world;

#define CONTINENT_GIRTH 1200.0f

void WorldGenerator::GenerateWorld(WorldScene &scene)
{
    auto map = scene.GetWorldMap();

    auto defineContinents = [&]
    {
        for(auto tile = map->tiles.GetStart(); tile != map->tiles.GetEnd(); ++tile)
        {
            if(map->GetCenterTile()->GetPhysicalDistanceTo(*tile) > CONTINENT_GIRTH)
            {
                tile->Type = WorldTiles::SEA;
            }
            else
            {
                tile->Type = WorldTiles::LAND;
            }
        }
    };

    auto generateRelief = [&] 
    {
        for(auto tile = map->tiles.GetStart(); tile != map->tiles.GetEnd(); ++tile)
        {
            if(tile->Type == WorldTiles::SEA)
            {
                tile->Relief = WorldReliefFactory::BuildRelief(WorldReliefs::SEA);
            }
            else
            {
                auto seaCount = 0;
                auto &nearbyTiles = tile->GetNearbyTiles(2);
                for(auto nearbyTileIterator = nearbyTiles.GetStart(); nearbyTileIterator != nearbyTiles.GetEnd(); ++nearbyTileIterator)
                {
                    auto nearbyTile = *nearbyTileIterator;
                    if(nearbyTile->Type == WorldTiles::SEA)
                        seaCount++;
                }

                if(seaCount == 0 && utility::GetRandom(1, 100) < 10)
                {
                    tile->Relief = WorldReliefFactory::BuildRelief(WorldReliefs::MOUNTAINS);
                }
                else
                {
                    tile->Relief = WorldReliefFactory::BuildRelief(WorldReliefs::PLAINS);
                }
            }
        }
    };

    auto uniteMountains = [&] 
    {
        container::Grid <bool> flags (map->tiles.GetWidth(), map->tiles.GetHeight());
        for(auto flag = flags.GetStart(); flag != flags.GetEnd(); ++flag)
        {
            *flag = false;
        }

        auto flag = flags.GetStart();
        for(auto tile = map->tiles.GetStart(); tile != map->tiles.GetEnd(); ++tile, ++flag)
        {
            if(tile->Type == WorldTiles::SEA)
                continue;

            if(tile->HasRelief(WorldReliefs::MOUNTAINS) == true)
                continue;

            auto mountainCount = 0;
            auto &nearbyTiles = tile->GetNearbyTiles(1);
            for(auto nearbyTileIterator = nearbyTiles.GetStart(); nearbyTileIterator != nearbyTiles.GetEnd(); ++nearbyTileIterator)
            {
                auto nearbyTile = *nearbyTileIterator;
                if(nearbyTile->HasRelief(WorldReliefs::MOUNTAINS))
                    mountainCount++;
            }

            if(mountainCount >= 2)
            {
                *flag = true;
            }
        }

        flag = flags.GetStart();
        for(auto tile = map->tiles.GetStart(); tile != map->tiles.GetEnd(); ++tile, ++flag)
        {
            if(*flag)
            {
                tile->Relief = WorldReliefFactory::BuildRelief(WorldReliefs::MOUNTAINS);
            }
        }
    };

    auto eraseIsolatedMountains = [&] 
    {
        container::Grid <bool> flags (map->tiles.GetWidth(), map->tiles.GetHeight());
        for(auto flag = flags.GetStart(); flag != flags.GetEnd(); ++flag)
        {
            *flag = false;
        }

        auto flag = flags.GetStart();
        for(auto tile = map->tiles.GetStart(); tile != map->tiles.GetEnd(); ++tile, ++flag)
        {
            if(tile->Type == WorldTiles::SEA)
                continue;

            if(tile->HasRelief(WorldReliefs::MOUNTAINS) == false)
                continue;

            auto mountainCount = 0;
            auto &nearbyTiles = tile->GetNearbyTiles(1);
            for(auto nearbyTileIterator = nearbyTiles.GetStart(); nearbyTileIterator != nearbyTiles.GetEnd(); ++nearbyTileIterator)
            {
                auto nearbyTile = *nearbyTileIterator;
                if(nearbyTile->HasRelief(WorldReliefs::MOUNTAINS))
                    mountainCount++;
            }

            if(mountainCount == 1)
            {
                *flag = true;
            }
        }

        flag = flags.GetStart();
        for(auto tile = map->tiles.GetStart(); tile != map->tiles.GetEnd(); ++tile, ++flag)
        {
            if(*flag)
            {
                tile->Relief = WorldReliefFactory::BuildRelief(WorldReliefs::PLAINS);
            }
        }
    };

    auto generateBiomes = [&]
    {
        for(auto tile = map->tiles.GetStart(); tile != map->tiles.GetEnd(); ++tile)
        {
            if(tile->Type == WorldTiles::SEA)
            {
                tile->Biome = WorldBiomeFactory::BuildBiome(WorldBiomes::MARINE);

                tile->fertility = 1;
                tile->industry = 0;
            }
            else
            {
                if(tile->HasRelief(WorldReliefs::MOUNTAINS))
                {
                    tile->Biome = WorldBiomeFactory::BuildBiome(WorldBiomes::DESERT);

                    tile->fertility = 0;
                    tile->industry = utility::GetRandom(1, 100) < 50 ? 2 : 1;   
                }
                else
                {
                    auto nearbyMountainCount = 0;
                    auto &nearbyTiles = tile->GetNearbyTiles(1);
                    for(auto nearbyTileIterator = nearbyTiles.GetStart(); nearbyTileIterator != nearbyTiles.GetEnd(); ++nearbyTileIterator)
                    {
                        auto nearbyTile = *nearbyTileIterator;
                        if(nearbyTile->HasRelief(WorldReliefs::MOUNTAINS))
                            nearbyMountainCount++;
                    }

                    auto farawayMountainCount = 0;
                    nearbyTiles = tile->GetNearbyTiles(2);
                    for(auto nearbyTileIterator = nearbyTiles.GetStart(); nearbyTileIterator != nearbyTiles.GetEnd(); ++nearbyTileIterator)
                    {
                        auto nearbyTile = *nearbyTileIterator;
                        if(nearbyTile->HasRelief(WorldReliefs::MOUNTAINS))
                            farawayMountainCount++;
                    }

                    if(nearbyMountainCount >= 1 || (farawayMountainCount >= 1 && utility::GetRandom(1, 100) < 50) || utility::GetRandom(1, 100) < 10)
                    {
                        tile->Biome = WorldBiomeFactory::BuildBiome(WorldBiomes::WOODS);

                        tile->fertility = 1;
                        tile->industry = utility::GetRandom(1, 100) < 50 ? 1 : 2;
                    }
                    else
                    {
                        if(utility::GetRandom(1, 100) < 65)
                        {
                            tile->Biome = WorldBiomeFactory::BuildBiome(WorldBiomes::STEPPE);

                            bool hasMaxFertility = utility::GetRandom(1, 100) < 5;
                            tile->fertility = hasMaxFertility ? 4 : utility::GetRandom(1, 100) < 35 ? 3 : 2;

                            tile->industry = hasMaxFertility ? 0 : utility::GetRandom(1, 100) < 50 ? 1 : 0;
                        }
                        else 
                        {
                            tile->Biome = WorldBiomeFactory::BuildBiome(WorldBiomes::DESERT);

                            tile->fertility = 0;
                            tile->industry = utility::GetRandom(1, 100) < 50 ? 1 : 0;
                        }
                    }
                }
            }
        }
    };

    auto initializeTiles = [&]
    {
        for(auto tile = map->tiles.GetStart(); tile != map->tiles.GetEnd(); ++tile)
        {
            tile->Initialize();
        }
    };

    defineContinents();

    generateRelief();

    uniteMountains();

    eraseIsolatedMountains();

    generateBiomes();

    initializeTiles();
}