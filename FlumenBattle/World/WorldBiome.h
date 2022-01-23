#pragma once

#include "FlumenBattle/World/Types.h"

namespace world
{
    struct WorldBiome
    {
        WorldBiomes Type;

        int TravelPenalty;
    };

    class WorldBiomeFactory
    {
    public:
        static const WorldBiome * BuildBiome(WorldBiomes type)
        {
            switch(type)
            {
                case WorldBiomes::DESERT:
                    return BuildDesertBiome();
                case WorldBiomes::STEPPE:
                    return BuildSteppeBiome();
                case WorldBiomes::WOODS:
                    return BuildWoodsBiome();
            }
        }

    private:
        static const WorldBiome * BuildDesertBiome()
        {
            static WorldBiome biome = {WorldBiomes::DESERT, 0};
            return &biome;
        }

        static const WorldBiome * BuildSteppeBiome()
        {
            static WorldBiome biome = {WorldBiomes::STEPPE, 0};
            return &biome;
        }

        static const WorldBiome * BuildWoodsBiome()
        {
            static WorldBiome biome = {WorldBiomes::WOODS, 1};
            return &biome;
        }
    };
}