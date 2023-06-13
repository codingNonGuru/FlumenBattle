#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenEngine/Utility/Color.hpp"

#include "FlumenBattle/World/Technology/Technology.h"

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

        science::TechnologyRoster technologyRoster;

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