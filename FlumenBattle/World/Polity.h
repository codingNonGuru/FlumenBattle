#pragma once

#include "FlumenCore/Conventions.hpp"
#include "FlumenCore/Container/HexGrid.h"

#include "FlumenBattle/World/Science/Types.h"

namespace world
{
    namespace settlement
    {
        class Settlement;
    }

    namespace science
    {
        class TechnologyRoster;
    }
}

namespace world::polity
{
    class Faction;
    struct FactionDecision;

    class Polity
    {
        friend class PolityAllocator;

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

        Pool <settlement::Settlement *> settlements;

        settlement::Settlement *ruler;

        Pool <Faction> factions;

        Integer malariaDeathCount;

        science::TechnologyRoster *technologyRoster;

        container::HexGrid <Interest> interestMap;

        void MapInterest(settlement::Settlement *);

    public:
        void Initialize(settlement::Settlement *);

        void ExtendRealm(settlement::Settlement *);

        void RemoveSettlement(settlement::Settlement *);

        void UndergoDivision(Faction *);

        void DecideResearch();

        container::Array <FactionDecision> &Update();

        Faction *FindFaction(settlement::Settlement *);
        
        void ApplyTechnologyModifiers(settlement::Settlement *) const;

        settlement::Settlement *GetRuler() const {return ruler;}

        Integer GetPopulation() const;

        Integer GetIndustrialPower() const;

        Integer GetScientificPower() const;

        Integer GetDeathCount() const {return malariaDeathCount;}

        container::HexGrid <Interest> &GetInterestMap() {return interestMap;}

        bool HasDiscoveredTechnology(science::Technologies) const;
    };
}