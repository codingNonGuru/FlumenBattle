#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenEngine/Utility/Color.hpp"

#include "FlumenBattle/World/Technology/Technology.h"
#include "FlumenBattle/World/Settlement/Modifier.h"

namespace world
{
    class WorldTile;
    class WorldScene;
    class Polity;

    namespace group
    {
        class GroupDynamics;
    }
}

namespace world::settlement
{
    struct Affliction;
    struct SettlementEvent;
    struct SettlementEventGenerator;
    class SettlementProduction;

    struct SettlementTile
    {
        world::WorldTile * Tile;

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
        IRRIGATION,
        LIBRARY
    };

    class Settlement
    {
        friend class world::Polity;

        friend class world::WorldScene;

        friend class AfflictionPerformer;

        friend class SettlementFactory;

        friend class SettlementAllocator;

        friend struct SettlementEventGenerator;

        friend class SettlementProductionFinisher;

        friend class SettlementLabel;

        Word name;

        Color banner;

        world::WorldTile *location;

        Integer population;

        group::GroupDynamics *groupDynamics;

        Integer growth;

        Integer cultureGrowth;

        science::TechnologyRoster technologyRoster;

        Pool <SettlementTile> tiles;

        world::Polity *polity;

        Pool <Affliction> afflictions;

        Pool <SettlementEvent> events;

        SettlementProduction *currentProduction;

        ModifierManager modifierManager;
        
        bool hasSewage;

        bool hasWalls;

        bool hasIrrigation;

        bool hasLibrary;

        void Initialize(Word, Color, world::WorldTile *);

        world::WorldTile * FindColonySpot();

        void WorkNewTile();

        void GrowBorders();

        void DecideProduction();

        void DecideResearch();

    public:
        Word GetName() const {return name;}

        Color GetBanner() const {return banner;}

        Color GetRulerBanner() const;

        world::WorldTile *GetLocation() const;

        Pool <SettlementTile> & GetTiles();

        FoodSecurity GetFoodSecurity() const;

        Integer GetFoodProduction() const;

        Integer GetIndustrialProduction() const;

        Integer GetScienceProduction() const;

        Integer GetPopulation() const {return population;}

        Integer GetGrowth() const {return growth;}

        Integer GetWorkedTiles() const;

        world::Polity *GetPolity() const {return polity;}

        int GetModifier(SettlementModifiers) const;

        void SetPolity(world::Polity *);

        void AddModifier(Modifier);

        void StrengthenPatrol();

        void Update();
    };
}