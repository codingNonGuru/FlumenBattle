#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenEngine/Utility/Color.hpp"

namespace world
{
    class WorldTile;
    class Polity;
    struct Affliction;
    struct SettlementEvent;
    struct SettlementEventGenerator;
    class SettlementProduction;
    namespace group
    {
        class GroupDynamics;
    }

    struct SettlementTile
    {
        WorldTile * Tile;

        bool IsWorked;

        bool IsBuilt;
    };

    enum class FoodSecurity
    {
        CORNUCOPIA,
        ABUNDANT,
        ENOUGH,
        BARELY_AVAILABLE,
        LACKING,
        SORELY_LACKING
    };

    enum class SettlementBuildings
    {
        SEWAGE,
        WALLS,
        GRANARY,
        MONUMENT,
        IRRIGATION
    };

    enum class Technologies
    {
        MASONRY, //+1 to building saving throws against earthquakes
        HAND_WASHING, //+1 to saving throws against disease
        FUR_CAMPS, //+1 food from forests
        TRAINED_SENTINELS, //+1 to attack rolls against bandits
        FISHERIES, //+1 food from oceans
        WAGON_BANDS, //+2 to colonization dice rolls
        FOOD_RATIONING, //+1 to saving throws against hunger
        COUNT
    };

    class TechnologyRoster
    {
        static constexpr int TECHNOLOGY_COUNT = (int)Technologies::COUNT;

        bool hasResearched[TECHNOLOGY_COUNT];

        TechnologyRoster()
        {
            for(auto &technology : hasResearched)
            {
                technology = false;
            }
        }

    public:
        bool HasDiscovered(Technologies technology) const
        {
            return hasResearched[(int)technology];
        }

        void DiscoverTechnology(Technologies technology) 
        {
            hasResearched[(int)technology] = true;
        }
    };

    class Settlement
    {
        friend class Polity;

        friend class AfflictionPerformer;

        friend class SettlementFactory;

        friend class SettlementAllocator;

        friend struct SettlementEventGenerator;

        friend class SettlementProductionFinisher;

        friend class SettlementLabel;

        friend class WorldScene;

        Word name;

        Color banner;

        WorldTile *location;

        Integer population;

        group::GroupDynamics *groupDynamics;

        Integer growth;

        Integer cultureGrowth;

        Integer science;

        TechnologyRoster technologyRoster;

        Pool <SettlementTile> tiles;

        Polity *polity;

        Pool <Affliction> afflictions;

        Pool <SettlementEvent> events;

        SettlementProduction *currentProduction;
        
        bool hasSewage;

        bool hasWalls;

        bool hasIrrigation;

        bool hasLibrary;

        void Initialize(Word, Color, WorldTile *);

        WorldTile * FindColonySpot();

        void WorkNewTile();

        void GrowBorders();

        void DecideProduction();

    public:
        Word GetName() const {return name;}

        Color GetBanner() const {return banner;}

        Color GetRulerBanner() const;

        WorldTile *GetLocation() const;

        Pool <SettlementTile> & GetTiles();

        FoodSecurity GetFoodSecurity() const;

        Integer GetFoodProduction() const;

        Integer GetIndustrialProduction() const;

        Integer GetPopulation() const {return population;}

        Integer GetGrowth() const {return growth;}

        Integer GetWorkedTiles() const;

        Polity *GetPolity() const {return polity;}

        void SetPolity(Polity *);

        void StrengthenPatrol();

        void Update();
    };
}