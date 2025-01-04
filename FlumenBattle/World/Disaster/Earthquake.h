#pragma once

#include "FlumenCore/Singleton.h"

namespace world
{
    namespace tile {struct WorldTile;}
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

        tile::WorldTile *epicenter;

        int range;

        int strength;

    public:
        Earthquake(tile::WorldTile *, int);

        void ApplyEffect() const;

        int GetDifficultyClass() const {return baseDifficultyClass + strength;}
    };

    class EarthquakeGenerator : public core::Singleton <EarthquakeGenerator>
    {
        friend class world::WorldScene;

        void Generate();
    };
}