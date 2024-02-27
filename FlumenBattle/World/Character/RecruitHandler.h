#pragma once

#include "FlumenCore/Singleton.h"
#include "FlumenCore/Container/Pool.hpp"

#include "FlumenBattle/World/Character/RecruitData.h"

namespace world::settlement
{
    class Settlement;
}

namespace world::character
{
    class RecruitPool
    {
        friend class RecruitHandler;

        settlement::Settlement *settlement;

        container::Pool <RecruitData> recruits;

    public:
        bool operator==(settlement::Settlement *other) {return this->settlement == other;}

        int GetPower() const;
    };

    class RecruitHandler : public core::Singleton <RecruitHandler>
    {
        friend class core::Singleton <RecruitHandler>;

        container::Pool <RecruitPool> recruitPools;

        RecruitHandler();

        void GeneratePool(RecruitPool *, settlement::Settlement *);

        static int GetPotentialPower(settlement::Settlement *);

    public:
        container::Pool <RecruitData> &GetRecruitPool(settlement::Settlement *);

        void RemoveRecruit(settlement::Settlement *, unsigned int);
    };
}