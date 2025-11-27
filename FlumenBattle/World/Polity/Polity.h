#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenEngine/Container/HexGrid.h"

#include "FlumenBattle/World/Science/Types.h"
#include "FlumenBattle/World/Polity/NeighborHandler.h"

namespace world
{
    namespace tile {struct WorldTile;}

    namespace settlement
    {
        class Settlement;
        enum class ProductionOptions;
    }

    namespace science
    {
        class TechnologyRoster;
        struct TechnologyType;
    }
}

namespace world::polity
{
    class Faction;
    struct FactionDecision;
    class Mind;
    class MachineMind;
    class HumanMind;

    class Polity
    {
        friend class PolityAllocator;

        friend class MachineMind;

        friend class HumanMind;

        struct Interest : public core::hex::Tile
        {
            friend class Polity;

        private:
            settlement::Settlement *Owner;

            int Distance;

            int Value {0};

            int Risk {0};

        public:
            Interest() {}

            Interest(Integer3 coordinates, settlement::Settlement *owner, int distance) : core::hex::Tile(coordinates), Owner(owner), Distance(distance) {}

            Integer GetCost() const {return Distance - Value;}

            Integer GetDistance() const {return Distance;}

            settlement::Settlement *GetOwner() const {return Owner;}
        };

        struct FusionData
        {
            Faction *First {nullptr};

            Faction *Second {nullptr};
        };

        bool isValid;

        int turnsUntilDeletion;

        int uniqueId;

        Mind* controller;

        Pool <settlement::Settlement *> settlements;

        settlement::Settlement *ruler;

        Pool <Faction> factions;

        Integer malariaDeathCount;

        science::TechnologyRoster *technologyRoster;

        container::HexGrid <Interest> interestMap;

        NeighborHandler neighborHandler;

        void MapInterest(settlement::Settlement *);

        FusionData CheckFactionMergers();

        void MergeFactions(FusionData);

    public:
        bool IsValid() const {return isValid;}

        int GetUniqueId() const {return uniqueId;}

        void Initialize(settlement::Settlement *, bool);

        void ExtendRealm(settlement::Settlement *);

        void RemoveSettlement(settlement::Settlement *);

        void UndergoDivision(Faction *);

        void UndergoDivision(settlement::Settlement *);

        void DecideResearch();

        void ProcessTrade();

        container::Array <FactionDecision> &Update();

        void Decide();

        Faction *FindFaction(settlement::Settlement *);
        
        void ApplyTechnologyModifiers(settlement::Settlement *) const;

        settlement::Settlement *GetRuler() const {return ruler;}

        void SetRuler(settlement::Settlement *newRuler) {ruler = newRuler;}

        Integer GetPopulation() const;

        Integer GetIndustrialPower() const;

        Integer GetScientificPower() const;

        Integer GetDeathCount() const {return malariaDeathCount;}

        const Pool <settlement::Settlement *> &GetSettlements() const {return settlements;}

        container::HexGrid <Interest> &GetInterestMap() {return interestMap;}

        const Pool <Faction> &GetFactions() {return factions;}

        const science::TechnologyType *GetResearchTarget() const;

        int GetResearchProgress() const;

        bool HasDiscoveredTechnology(science::Technologies) const;

        void AddTechnology(science::Technologies);

        const container::Array <world::settlement::Settlement *> &GetSecederNeighbours() const;

        bool IsPlayerControlled() const;

        void SetController(bool);

        void CleanUp();

        void MarkForDeletion();

        bool ShouldBeDeleted() const {return isValid == false && turnsUntilDeletion == 0;}

        void RegisterPopIncrease(settlement::Settlement *);

        void RegisterTileExplored(settlement::Settlement *, tile::WorldTile *);

        void RegisterProductionFinished(settlement::Settlement *, settlement::ProductionOptions);

        void RegisterSettlementDeletion(settlement::Settlement *);

        void RecalculateLinks();

        const container::Array <Neighbor> &GetNeighbors() const {return neighborHandler.neighbors;}
    };
}