#pragma once

#include "FlumenCore/Conventions.hpp"

namespace world
{
    class Settlement;

    class Polity
    {
        Pool <Settlement *> settlements;

        Settlement *ruler;

        Integer malariaDeathCount;

    public:
        void Initialize(Settlement *);

        void ExtendRealm(Settlement *);

        Settlement *GetRuler() const {return ruler;}

        Integer GetPopulation() const;

        Integer GetIndustrialPower() const;

        Integer GetDeathCount() const {return malariaDeathCount;}

        void Update();
    };
}