#include "FlumenEngine/Utility/Perlin.hpp"

#include "WorldGenerator.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Tile/WorldMap.h"
#include "FlumenBattle/World/Tile/WorldTile.h"
#include "FlumenBattle/World/Tile/WorldBiome.h"
#include "FlumenBattle/World/Tile/WorldRelief.h"
#include "FlumenBattle/World/WorldAllocator.h"
#include "FlumenBattle/World/Settlement/SettlementAllocator.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Group/GroupAllocator.h"
#include "FlumenBattle/World/Group/GroupFactory.h"
#include "FlumenBattle/World/Group/GroupCore.h"
#include "FlumenBattle/World/Group/Types.h"
#include "FlumenBattle/PreGame/Types.h"
#include "FlumenBattle/World/Settlement/Types.h"
#include "FlumenBattle/World/Polity/PolityAllocator.h"
#include "FlumenBattle/World/SimulationMap.h"
#include "FlumenBattle/World/Polity/Polity.h"
#include "FlumenBattle/PreGame/PartyLoader.h"
#include "FlumenBattle/PreGame/Types.h"
#include "FlumenBattle/World/Tile/River.h"
#include "FlumenBattle/World/RiverGenerator.h"

using namespace world;

#define METAL_SPAWN_CHANCE 1

static const auto MAXIMUM_TILE_ELEVATION = 1000;

static const auto SIZE_TO_GIRTH_FACTOR = 12.0f;

const auto SCRUBLAND_HEAT_THRESHOLD = 35;

int WorldGenerator::GenerateWorld(
    pregame::NewWorldData data, 
    const container::Grid <float> &perlinNoise, 
    const container::Grid <float> &snowNoise,
    const container::Grid <float> &desertNoise,
    const container::Grid <float> &forestNoise
    )
{
    assert((data.Size % TILES_PER_SIMULATION_DOMAIN == 0) && "World generation size incompatible with simulation standard.\n");

    std::cout<<"World has started generating\n";

    auto &scene = *WorldScene::Get();

    scene.worldMap = new tile::WorldMap(data.Size);

    auto map = scene.worldMap;

    const float CONTINENT_GIRTH = float(data.Size) * SIZE_TO_GIRTH_FACTOR;

    auto defineContinents = [&]
    {
        std::cout<<"Defining continents\n";

        auto centerTile = map->GetCenterTile();
        for(auto tile = map->tiles.GetStart(); tile != map->tiles.GetEnd(); ++tile)
        {
            auto distanceToCenter = centerTile->GetPhysicalDistanceTo(*tile);
            auto angleBetweenTiles = centerTile->GetPhysicalAngle(*tile);
            auto baseHeight = exp(-(distanceToCenter * distanceToCenter) / (2.0f * CONTINENT_GIRTH * CONTINENT_GIRTH));

            auto angleFactor = (1.0f + cos(angleBetweenTiles * 5.0f)) * 0.5f;
            angleFactor *= angleFactor * angleFactor;
            angleFactor = angleFactor * 2.5f + 0.05f;

            baseHeight = (angleFactor * (1.0f - baseHeight) * (1.0f - baseHeight) + baseHeight) * baseHeight;

            auto noiseFactor = *perlinNoise.Get(tile->SquareCoordinates.x, tile->SquareCoordinates.y);

            auto height = noiseFactor + (1.0f - noiseFactor) * baseHeight;
            height *= 0.4f + baseHeight * 0.6f;

            height = height * 0.4f + height * noiseFactor * 0.6f;
            
            if(height < 0.5f)
            {
                tile->Type = WorldTiles::SEA;
            }
            else
            {
                tile->Type = WorldTiles::LAND;
            }

            tile->Elevation = int(height * float(MAXIMUM_TILE_ELEVATION));
        }
    };

    auto generateRelief = [&] 
    {
        auto centerTile = map->GetCenterTile();
        for(auto tile = map->tiles.GetStart(); tile != map->tiles.GetEnd(); ++tile)
        {
            auto distanceToCenter = centerTile->GetPhysicalDistanceTo(*tile);
            auto angleBetweenTiles = centerTile->GetPhysicalAngle(*tile);
            auto baseHeight = exp(-(distanceToCenter * distanceToCenter) / (2.0f * CONTINENT_GIRTH * CONTINENT_GIRTH));
            auto baseHeight2 = exp(-(distanceToCenter) / (2.8f * CONTINENT_GIRTH));

            auto angleFactor = (1.0f + cos(angleBetweenTiles * 5.0f)) * 0.5f;
            angleFactor = 1.0f - angleFactor;

            angleFactor = exp(-angleFactor / (0.5f * baseHeight));
            angleFactor = utility::Clamp(angleFactor, 0.0f, 1.0f);

            baseHeight = (angleFactor * (1.0f - baseHeight) + baseHeight) * baseHeight2;

            auto noiseFactor = *perlinNoise.Get(tile->SquareCoordinates.x, tile->SquareCoordinates.y);

            auto height = noiseFactor + (1.0f - noiseFactor) * baseHeight;
            height *= 0.4f + baseHeight * 0.6f;

            height *= height;

            //tile->Elevation = int(height * float(MAXIMUM_TILE_ELEVATION));

            if(tile->Type == WorldTiles::SEA)
            {
                tile->Relief = tile::WorldReliefFactory::BuildRelief(WorldReliefs::SEA);
            }
            else
            {
                auto mountainChance = int(height * 100.0f) - 15;
                if(mountainChance < 0)
                    mountainChance = 0;

                mountainChance /= 3;

                //mountainChance = 0;

                if(utility::GetRandom(1, 100) <= mountainChance)
                {
                    tile->Relief = tile::WorldReliefFactory::BuildRelief(WorldReliefs::MOUNTAINS);
                }
                else
                {
                    tile->Relief = tile::WorldReliefFactory::BuildRelief(WorldReliefs::PLAINS);
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
            auto nearbyTiles = tile->GetNearbyTiles();
            for(auto &nearbyTile : nearbyTiles)
            {
                if(nearbyTile == nullptr)
                    continue;

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
                tile->Relief = tile::WorldReliefFactory::BuildRelief(WorldReliefs::MOUNTAINS);
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
            auto nearbyTiles = tile->GetNearbyTiles(1);
            for(auto &nearbyTile : nearbyTiles)
            {
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
                tile->Relief = tile::WorldReliefFactory::BuildRelief(WorldReliefs::PLAINS);
            }
        }
    };

    auto generateClimates = [&]
    {
        for(auto tile = map->tiles.GetStart(); tile != map->tiles.GetEnd(); ++tile)
        {
            auto y = tile->SquareCoordinates.y;
            y -= map->tiles.GetHeight() / 2;
            
            float baseHeatFactor = float(y) / float(map->tiles.GetHeight() / 2);
            float b = 0.4f;
            baseHeatFactor = exp(-(baseHeatFactor * baseHeatFactor) / (2.0f * b * b));

            auto snowFactor = *snowNoise.Get(tile->SquareCoordinates.x, tile->SquareCoordinates.y);

            auto heatFactor = snowFactor + (1.0f - snowFactor) * baseHeatFactor;
            heatFactor *= 0.5f + baseHeatFactor * 0.5f;

            auto desertValue = *desertNoise.Get(tile->SquareCoordinates.x, tile->SquareCoordinates.y);

            heatFactor = desertValue * heatFactor * 0.35f + heatFactor * 0.65f;

            tile->Heat = int(heatFactor * float(tile::WorldTile::MAXIMUM_TILE_HEAT));

            auto getMountainCount = [&] (int range)
            {
                auto count = 0;

                auto nearbyTiles = tile->GetNearbyTiles(range);
                for(auto &nearbyTile : nearbyTiles)
                {
                    if(nearbyTile->HasRelief(WorldReliefs::MOUNTAINS))
                        count++;
                }

                return count;
            };

            auto nearbyMountainCount = getMountainCount(1);
            auto farawayMountainCount = getMountainCount(2);
            auto veryFarawayMountainCount = getMountainCount(3);
            auto extremelyFarawayMountainCount = getMountainCount(4);

            tile->Heat -= nearbyMountainCount;
            tile->Heat -= farawayMountainCount / 2;
            tile->Heat -= veryFarawayMountainCount / 3;
            tile->Heat -= extremelyFarawayMountainCount / 5;
        }
    };

    auto generateBiomes = [&]
    {
        for(auto tile = map->tiles.GetStart(); tile != map->tiles.GetEnd(); ++tile)
        {
            if(tile->Type == WorldTiles::SEA)
            {
                tile->Biome = tile::WorldBiomeFactory::BuildBiome(WorldBiomes::MARINE);

                tile->SetResource(settlement::ResourceTypes::FOOD, 1);
                tile->SetResource(settlement::ResourceTypes::TIMBER, 0);
                tile->SetResource(settlement::ResourceTypes::METAL, 0);
            }
            else
            {
                tile->SetResource(settlement::ResourceTypes::METAL, (utility::RollD100Dice() > 100 - METAL_SPAWN_CHANCE) ? 1 : 0);



                {
                    auto forestValue = *forestNoise.Get(tile->SquareCoordinates.x, tile->SquareCoordinates.y);

                    auto heatFactor = (float)tile->Heat - 60.0f;
                    heatFactor = exp(-heatFactor * heatFactor / (2.0f * 13.0f * 13.0f));

                    forestValue *= heatFactor;

                    auto getMountainCount = [&] (int range)
                    {
                        auto count = 0;

                        auto nearbyTiles = tile->GetNearbyTiles(range);
                        for(auto &nearbyTile : nearbyTiles)
                        {
                            if(nearbyTile->HasRelief(WorldReliefs::MOUNTAINS))
                                count++;
                        }

                        return count;
                    };

                    auto nearbyMountainCount = getMountainCount(1) * 20;
                    auto farawayMountainCount = getMountainCount(2);

                    bool isOneMountainNearby = farawayMountainCount > 0;

                    farawayMountainCount = (farawayMountainCount - 1) * 5;
                    if(farawayMountainCount < 0)
                        farawayMountainCount = 0;

                    if(forestValue > 0.45f || (isOneMountainNearby && utility::GetRandom(1, 100) < nearbyMountainCount + farawayMountainCount))
                    {
                        tile->Biome = tile::WorldBiomeFactory::BuildBiome(WorldBiomes::WOODS);

                        tile->SetResource(settlement::ResourceTypes::FOOD, utility::RollD100Dice() < 50 ? 1 : 0);
                        tile->SetResource(settlement::ResourceTypes::TIMBER, utility::GetRandom(1, 100) < 50 ? 2 : 3);
                    }
                    else
                    {
                        if(tile->Heat < 80)
                        {
                            tile->Biome = tile::WorldBiomeFactory::BuildBiome(WorldBiomes::STEPPE);

                            tile->IsScrubland = tile->Heat < SCRUBLAND_HEAT_THRESHOLD;

                            bool hasMaxFertility = utility::GetRandom(1, 100) < 5;
                            if(tile->IsScrubland == true)
                            {
                                hasMaxFertility = false;
                            }

                            auto food = hasMaxFertility == true ? 4 : utility::GetRandom(1, 100) < 35 ? 3 : 2;
                            if(tile->IsScrubland == true)
                            {
                                food -= 1;
                            }

                            tile->SetResource(settlement::ResourceTypes::FOOD, food);

                            auto timber = hasMaxFertility == true ? 0 : utility::GetRandom(1, 100) < 50 ? 1 : 0;
                            if(tile->IsScrubland == true)
                            {
                                timber = 0;
                            }

                            tile->SetResource(settlement::ResourceTypes::TIMBER, timber);
                        }
                        else 
                        {
                            tile->Biome = tile::WorldBiomeFactory::BuildBiome(WorldBiomes::DESERT);

                            tile->SetResource(settlement::ResourceTypes::FOOD, 0);
                            tile->SetResource(settlement::ResourceTypes::TIMBER, utility::GetRandom(1, 100) < 50 ? 1 : 0);
                        }
                    }
                }

                if(tile->HasRelief(WorldReliefs::MOUNTAINS))
                {
                    if(tile->HasBiome(WorldBiomes::WOODS) == true)
                    { 
                        tile->Biome = tile::WorldBiomeFactory::BuildBiome(WorldBiomes::STEPPE);
                    }

                    tile->SetResource(settlement::ResourceTypes::FOOD, 0);
                    tile->SetResource(settlement::ResourceTypes::TIMBER, utility::GetRandom(1, 100) < 50 ? 2 : 1);
                }
            }
        }

        //Set fiber resource abundance
        for(auto &tile : map->tiles)
        {
            if(tile.Type == WorldTiles::SEA)
            {
                tile.SetResource(settlement::ResourceTypes::FIBER, 0);
            }
            else
            {
                if(tile.HasRelief(WorldReliefs::MOUNTAINS) == true)
                {
                    tile.SetResource(settlement::ResourceTypes::FIBER, utility::GetRandom(1, 2));
                }
                else if(tile.HasRelief(WorldReliefs::HILLS) == true)
                {
                    tile.SetResource(settlement::ResourceTypes::FIBER, utility::GetRandom(1, 4));
                }
                else if(tile.HasRelief(WorldReliefs::PLAINS) == true)
                {
                    tile.SetResource(settlement::ResourceTypes::FIBER, utility::GetRandom(0, 1));
                }
            }
        }

        //Set clay resource abundance
        for(auto &tile : map->tiles)
        {
            if(tile.Type == WorldTiles::SEA)
            {
                tile.SetResource(settlement::ResourceTypes::CLAY, 0);
            }
            else
            {
                if(tile.HasRelief(WorldReliefs::MOUNTAINS) == true)
                {
                    tile.SetResource(settlement::ResourceTypes::CLAY, 0);
                }
                else if(tile.HasRelief(WorldReliefs::HILLS) == true)
                {
                    tile.SetResource(settlement::ResourceTypes::CLAY, utility::GetRandom(0, 3));
                }
                else if(tile.HasRelief(WorldReliefs::PLAINS) == true)
                {
                    tile.SetResource(settlement::ResourceTypes::CLAY, utility::GetRandom(0, 2));
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

    generateClimates();

    generateBiomes();

    initializeTiles();

    RiverGenerator::Get()->DefineRivers();

    GenerateSociety(data);

    std::cout<<"World has finished generating\n";
}

void WorldGenerator::GenerateSociety(pregame::NewWorldData data)
{
    static auto &scene = *WorldScene::Get();

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
    
    settlement::Settlement *newSettlement = nullptr;

    for(auto i = 0; i < 1; ++i)
    {
        auto location = findSettleLocation();
        newSettlement = scene.FoundSettlement(location, RaceTypes::DWARF, nullptr);
    }

    auto groups = group::GroupAllocator::Get()->GetGroups();
    scene.groups = groups;

    auto battles = group::GroupAllocator::Get()->GetBattles();
    scene.battles = battles;
}

void WorldGenerator::GeneratePlayerGroup(const container::Array <pregame::MemberData> &partyMemberData)
{
    static auto &scene = *WorldScene::Get();

    auto settlements = settlement::SettlementAllocator::Get()->GetSettlements();

    scene.playerGroup = group::GroupFactory::CreatePlayerGroup({group::GroupClasses::PLAYER, RaceTypes::HUMAN, scene.settlements->GetRandom(), &partyMemberData});

    SimulationMap::Get()->Initialize();

    for(auto &settlement : *settlements)
    {
        settlement.SetupSimulation();
    }

    auto homeSettlement = settlements->GetRandom();

    //homeSettlement->GetPolity()->SetController(true);
    //scene.playerGroup->SetDomain(homeSettlement->GetPolity());

    scene.playerGroup->GetTile()->RevealTile();
    for(auto &tile : scene.playerGroup->GetTile()->GetNearbyTiles(2).Tiles)
    {
        tile->RevealTile();
    }
}

int WorldGenerator::GetMaximumPolityCount(int worldSize) const
{
    return worldSize;
}

int WorldGenerator::GetMaximumSettlementCount(int worldSize) const
{
    return (worldSize * worldSize) / 80;
}

int WorldGenerator::GetMaximumGroupCount(int worldSize) const
{
    return (worldSize * worldSize) / 60;
}