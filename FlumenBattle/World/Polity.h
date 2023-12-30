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

    class Polity
    {
        struct Interest : public core::hex::Tile
        {
            settlement::Settlement *Owner;

            int Distance;

            Interest() {}

            Interest(Integer3 coordinates, settlement::Settlement *owner, int distance) : core::hex::Tile(coordinates), Owner(owner), Distance(distance) {}
        };

        Pool <settlement::Settlement *> settlements;

        settlement::Settlement *ruler;

        Integer malariaDeathCount;

        science::TechnologyRoster *technologyRoster;

        container::HexGrid <Interest> interestMap;

    public:
        void Initialize(settlement::Settlement *);

        void ExtendRealm(settlement::Settlement *);

        void DecideResearch();

        void Update();
        
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