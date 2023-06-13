#pragma once

#include "FlumenCore/Conventions.hpp"

namespace world
{
    namespace settlement
    {
        class Settlement;
    }

    class Polity
    {
        Pool <settlement::Settlement *> settlements;

        settlement::Settlement *ruler;

        Integer malariaDeathCount;

    public:
        void Initialize(settlement::Settlement *);

        void ExtendRealm(settlement::Settlement *);

        settlement::Settlement *GetRuler() const {return ruler;}

        Integer GetPopulation() const;

        Integer GetIndustrialPower() const;

        Integer GetDeathCount() const {return malariaDeathCount;}

        void Update();
    };
}