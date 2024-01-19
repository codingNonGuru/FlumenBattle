#include "FlumenEngine/Utility/Perlin.hpp"

#include "WorldGenerator.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/WorldMap.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/WorldBiome.h"
#include "FlumenBattle/World/WorldRelief.h"
#include "FlumenBattle/World/WorldAllocator.h"
#include "FlumenBattle/World/Settlement/SettlementAllocator.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Group/GroupAllocator.h"
#include "FlumenBattle/World/Group/GroupFactory.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Group/Types.h"
#include "FlumenBattle/PreGame/Types.h"
#include "FlumenBattle/World/Settlement/Types.h"
#include "FlumenBattle/World/PolityAllocator.h"
#include "FlumenBattle/World/SimulationMap.h"

using namespace world;

#define METAL_SPAWN_CHANCE 1

int WorldGenerator::GenerateWorld(pregame::NewWorldData data, const container::Grid <float> &perlinNoise)
{
    assert((data.Size % TILES_PER_SIMULATION_DOMAIN == 0) && "World generation size incompatible with simulation standard.\n");

    std::cout<<"World has started generating\n";

    auto &scene = *WorldScene::Get();

    scene.worldMap = new WorldMap(data.Size);

    auto map = scene.worldMap;

    const float CONTINENT_GIRTH = float(data.Size) * 17.0f;

    auto defineContinents = [&]
    {
        std::cout<<"Defining continents\n";

        auto centerTile = map->GetCenterTile();
        for(auto tile = map->tiles.GetStart(); tile != map->tiles.GetEnd(); ++tile)
        {
            auto distanceToCenter = centerTile->GetPhysicalDistanceTo(*tile);
            auto baseHeight = exp(-(distanceToCenter * distanceToCenter) / (2.0f * CONTINENT_GIRTH * CONTINENT_GIRTH));

            auto noiseFactor = *perlinNoise.Get(tile->SquareCoordinates.x, tile->SquareCoordinates.y);

            auto height = noiseFactor + (1.0f - noiseFactor) * baseHeight;
            height *= 0.3f + baseHeight * 0.7f;
            if(height < 0.5f)
            {
                tile->Type = WorldTiles::SEA;
            }
            else
            {
                tile->Type = WorldTiles::LAND;
            }

            tile->Elevation = int(height * 100.0f);
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
                auto mountainChance = tile->Elevation - 50;
                if(mountainChance < 0)
                    mountainChance = 0;

                mountainChance /= 2;

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

                tile->SetResource(settlement::ResourceTypes::FOOD, 1);
                tile->SetResource(settlement::ResourceTypes::TIMBER, 0);
                tile->SetResource(settlement::ResourceTypes::METAL, 0);
            }
            else
            {
                tile->SetResource(settlement::ResourceTypes::METAL, (utility::RollD100Dice() > 100 - METAL_SPAWN_CHANCE) ? 1 : 0);

                if(tile->HasRelief(WorldReliefs::MOUNTAINS))
                {
                    tile->Biome = WorldBiomeFactory::BuildBiome(WorldBiomes::DESERT);

                    tile->SetResource(settlement::ResourceTypes::FOOD, 0);
                    tile->SetResource(settlement::ResourceTypes::TIMBER, utility::GetRandom(1, 100) < 50 ? 2 : 1);
                }
                else
                {
                    auto nearbyMountainCount = [&] ()
                    {
                        auto count = 0;
                        auto &nearbyTiles = tile->GetNearbyTiles(1);
                        for(auto &tile : nearbyTiles)
                        {
                            if(tile->HasRelief(WorldReliefs::MOUNTAINS))
                                count++;
                        }
                        return count;
                    } ();

                    auto farawayMountainCount = 0;
                    auto &nearbyTiles = tile->GetNearbyTiles(2);
                    for(auto &tile : nearbyTiles)
                    {
                        if(tile->HasRelief(WorldReliefs::MOUNTAINS))
                            farawayMountainCount++;
                    }

                    if(nearbyMountainCount >= 1 || (farawayMountainCount >= 1 && utility::GetRandom(1, 100) < 50) || utility::GetRandom(1, 100) < 10)
                    {
                        tile->Biome = WorldBiomeFactory::BuildBiome(WorldBiomes::WOODS);

                        tile->SetResource(settlement::ResourceTypes::FOOD, 1);
                        tile->SetResource(settlement::ResourceTypes::TIMBER, utility::GetRandom(1, 100) < 50 ? 1 : 2);
                    }
                    else
                    {
                        if(utility::GetRandom(1, 100) < 65)
                        {
                            tile->Biome = WorldBiomeFactory::BuildBiome(WorldBiomes::STEPPE);

                            bool hasMaxFertility = utility::GetRandom(1, 100) < 5;
                            tile->SetResource(settlement::ResourceTypes::FOOD, hasMaxFertility ? 4 : utility::GetRandom(1, 100) < 35 ? 3 : 2);
                            tile->SetResource(settlement::ResourceTypes::TIMBER, hasMaxFertility ? 0 : utility::GetRandom(1, 100) < 50 ? 1 : 0);
                        }
                        else 
                        {
                            tile->Biome = WorldBiomeFactory::BuildBiome(WorldBiomes::DESERT);

                            tile->SetResource(settlement::ResourceTypes::FOOD, 0);
                            tile->SetResource(settlement::ResourceTypes::TIMBER, utility::GetRandom(1, 100) < 50 ? 1 : 0);
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

    std::cout<<"World has finished generating\n";
}

void WorldGenerator::GenerateSociety(pregame::NewWorldData data)
{
    auto &scene = *WorldScene::Get();

    auto worldMap = scene.worldMap;

    WorldAllocator::Get()->AllocateSociety(data.Size);

    auto polities = polity::PolityAllocator::Get()->GetPolities();
    scene.polities = polities;

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
    
    for(auto i = 0; i < 1; ++i)
    {
        auto location = findSettleLocation();
        scene.FoundSettlement(location, nullptr);
    }

    auto groups = group::GroupAllocator::Get()->GetGroups();
    scene.groups = groups;

    auto battles = group::GroupAllocator::Get()->GetBattles();
    scene.battles = battles;

    scene.playerGroup = group::GroupFactory::Create({group::GroupClasses::PLAYER, RaceTypes::HUMAN, scene.settlements->GetRandom()});

    SimulationMap::Get()->Initialize();
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
    return (worldSize * worldSize) / 20;
}