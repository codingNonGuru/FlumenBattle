#include "WorldGenerator.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/WorldMap.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/WorldBiome.h"
#include "FlumenBattle/World/WorldRelief.h"
#include "FlumenBattle/World/WorldAllocator.h"
#include "FlumenBattle/World/Settlement/SettlementAllocator.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/GroupAllocator.h"
#include "FlumenBattle/World/Group/GroupFactory.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/PreGame/Types.h"

using namespace world;

#define METAL_SPAWN_CHANCE 1

int WorldGenerator::GenerateWorld(pregame::NewWorldData data)
{
    std::cout<<"World has started generating\n";

    auto &scene = *WorldScene::Get();

    scene.worldMap = new WorldMap(data.Size);

    auto map = scene.worldMap;

    const float CONTINENT_GIRTH = float(data.Size) * 23.0f;

    auto defineContinents = [&]
    {
        auto centerTile = map->GetCenterTile();
        for(auto tile = map->tiles.GetStart(); tile != map->tiles.GetEnd(); ++tile)
        {
            if(centerTile->GetPhysicalDistanceTo(*tile) > CONTINENT_GIRTH)
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
        auto centerTile = map->GetCenterTile();
        for(auto tile = map->tiles.GetStart(); tile != map->tiles.GetEnd(); ++tile)
        {
            if(tile->Type == WorldTiles::SEA)
            {
                tile->Relief = WorldReliefFactory::BuildRelief(WorldReliefs::SEA);
            }
            else
            {
                auto mountainChance = 20;
                auto distance = centerTile->GetPhysicalDistanceTo(*tile);
                if(distance > CONTINENT_GIRTH * 0.95f)
                {
                    mountainChance = 0;
                }
                else if(distance > CONTINENT_GIRTH * 0.9f)
                {
                    mountainChance = 4;
                }
                else if(distance > CONTINENT_GIRTH * 0.85f)
                {
                    mountainChance = 8;
                }
                else if(distance > CONTINENT_GIRTH * 0.7f)
                {
                    mountainChance = 12;
                }
                else if(distance > CONTINENT_GIRTH * 0.5f)
                {
                    mountainChance = 16;
                }

                if(utility::GetRandom(1, 100) <= mountainChance)
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
                if(mountainCount >= 4)
                    *flag = true;
                else if(utility::GetRandom(1, 100) > 50)
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

            if(mountainCount == 1 || mountainCount == 2)
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
                tile->metal = 0;
            }
            else
            {
                tile->metal = (utility::RollD100Dice() > 100 - METAL_SPAWN_CHANCE) ? 1 : 0;

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

    eraseIsolatedMountains();

    uniteMountains();

    uniteMountains();

    uniteMountains();

    generateBiomes();

    initializeTiles();

    GenerateSociety(data);

    hasFinishedGenerating = true;

    std::cout<<"World has finished generating\n";
}

void WorldGenerator::GenerateSociety(pregame::NewWorldData data)
{
    auto &scene = *WorldScene::Get();

    auto worldMap = scene.worldMap;

    WorldAllocator::Get()->AllocateSociety(data.Size);

    auto settlements = settlement::SettlementAllocator::Get()->GetSettlements();
    scene.settlements = settlements;

    auto paths = settlement::SettlementAllocator::Get()->GetPaths();
    scene.paths = paths;

    auto pathSegments = settlement::SettlementAllocator::Get()->GetPathSegments();
    scene.pathSegments = pathSegments;

    auto findSettleLocation = [&]
    {
        while(true)
        {
            auto tile = worldMap->GetEmptyRandomTile();

            bool isSettlementNearby = false;
            for(auto &settlement : *settlements)
            {
                auto distance = tile->GetDistanceTo(*settlement.GetLocation());
                if(distance < data.Size / 6)
                {
                    isSettlementNearby = true;
                    break;
                }
            }
            if(isSettlementNearby)
                continue;

            if(tile->HasBiome(WorldBiomes::STEPPE) && tile->GetSettlement() == nullptr)
                return tile;
        }
    };
    
    for(auto i = 0; i < 1/*data.Size / 10*/; ++i)
    {
        auto location = findSettleLocation();
        scene.FoundSettlement(location, nullptr, nullptr);
    }

    auto groups = GroupAllocator::Get()->GetGroups();
    scene.groups = groups;

    auto battles = GroupAllocator::Get()->GetBattles();
    scene.battles = battles;

    group::GroupFactory::Create({group::GroupTypes::COMPUTER, RaceTypes::ORC})->SetTile(worldMap->GetEmptyRandomTile());
}

int WorldGenerator::GetMaximumPolityCount(int worldSize) const
{
    return worldSize;
}

int WorldGenerator::GetMaximumSettlementCount(int worldSize) const
{
    return (worldSize * worldSize) / 15;
}

int WorldGenerator::GetMaximumGroupCount(int worldSize) const
{
    return (worldSize * worldSize) / 25;
}