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

        const settlement::Settlement *settlement;

        container::Pool <RecruitData> recruits;

        int timeSinceLastVisit;

        bool UpdateRemovals();

        bool UpdateAdditions();

    public:
        bool operator==(const settlement::Settlement *other) {return this->settlement == other;}

        int GetPower() const;

        bool Update();

        void AddRecruit();
    };

    class RecruitHandler : public core::Singleton <RecruitHandler>
    {
        friend class core::Singleton <RecruitHandler>;

        friend class RecruitPool;

        container::Pool <RecruitPool> recruitPools;

        RecruitHandler();

        void GeneratePool(RecruitPool *, const settlement::Settlement *);

        static int GetPotentialPower(const settlement::Settlement *);

        static int GetMaximumLevel(const settlement::Settlement *);

    public:
        container::Pool <RecruitData> &GetRecruitPool(const settlement::Settlement *);

        void RemoveRecruit(const settlement::Settlement *, unsigned int);

        bool Update(const settlement::Settlement *);
    };
}