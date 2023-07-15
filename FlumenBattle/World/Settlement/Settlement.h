#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenEngine/Utility/Color.hpp"

#include "FlumenBattle/World/Settlement/Modifier.h"
#include "FlumenBattle/World/Settlement/Types.h"

namespace world
{
    class WorldTile;
    class WorldScene;
    class Polity;

    namespace group
    {
        class GroupDynamics;
    }

    namespace disaster
    {
        class Earthquake;
    }
}

namespace world::settlement
{
    struct Affliction;
    struct SettlementEvent;
    struct SettlementEventGenerator;
    class SettlementProduction;
    class ConditionManager;
    struct ConditionData;
    class BuildingManager;

    struct SettlementTile
    {
        world::WorldTile * Tile;

        bool IsWorked;

        bool IsBuilt;
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

        friend class BuildingDamager;

        Word name;

        Color banner;

        world::WorldTile *location;

        Integer population;

        group::GroupDynamics *groupDynamics;

        Integer growth;

        Integer cultureGrowth;

        Pool <SettlementTile> tiles;

        world::Polity *polity;

        Pool <Affliction> afflictions;

        Pool <SettlementEvent> events;

        SettlementProduction *currentProduction;

        ModifierManager modifierManager;

        ConditionManager *conditionManager;
        
        BuildingManager *buildingManager;

        void Initialize(Word, Color, world::WorldTile *);

        world::WorldTile * FindColonySpot();

        void WorkNewTile();

        void GrowBorders();

        void DecideProduction();

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

        int GetModifier(Modifiers) const;

        void SetPolity(world::Polity *);

        void AddModifier(Modifier);

        void AddCondition(ConditionData);

        void AddBuilding(BuildingTypes);

        void StrengthenPatrol();

        void ProcessEarthquake(const disaster::Earthquake &);

        void Update();
    };
}