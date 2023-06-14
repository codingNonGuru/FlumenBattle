#pragma once

#include "FlumenCore/Conventions.hpp"

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
        Pool <settlement::Settlement *> settlements;

        settlement::Settlement *ruler;

        Integer malariaDeathCount;

        science::TechnologyRoster *technologyRoster;

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

        bool HasDiscoveredTechnology(science::Technologies) const;
    };
}