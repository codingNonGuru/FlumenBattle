#pragma once

#include "FlumenEngine/Core/Singleton.h"

namespace world
{
    class WorldTile;
    class WorldScene;
}

namespace world::settlement
{
    class Settlement;
}

namespace world::disaster
{
    class Earthquake
    {
        static const int baseDifficultyClass = 2;

        WorldTile *epicenter;

        int range;

        int strength;

    public:
        Earthquake(WorldTile *, int);

        void ApplyEffect() const;

        int GetDifficultyClass() const {return baseDifficultyClass + strength;}
    };

    class EarthquakeGenerator : public Singleton <EarthquakeGenerator>
    {
        friend class world::WorldScene;

        void Generate();
    };
}