#pragma once

#include "FlumenCore/Container/Pool.hpp"

namespace world
{
    namespace settlement
    {
        class Settlement;
    }
}

namespace world::polity
{
    class Faction;
    class Polity;

    enum class FactionDecisions
    {
        DECLARE_INDEPENDENCE,
        NONE
    };

    struct FactionDecision
    {
        FactionDecisions Decision;

        polity::Faction *Faction {nullptr};
    };

    class Faction
    {
        friend class PolityAllocator;

        settlement::Settlement *leader;

        container::Pool <settlement::Settlement *> members;

        Polity *polity;

        int independenceDrive {0};

        bool isActive {true};

    public:
        const container::Pool <settlement::Settlement *> &GetMembers() const {return members;}

        settlement::Settlement *GetLeader() const {return leader;}

        Polity *GetPolity() const {return polity;}

        void Initialize(Polity *newPolity) 
        {
            independenceDrive = 0;

            polity = newPolity;

            isActive = true;
        }

        void AddMember(settlement::Settlement *);

        void SetLeader(settlement::Settlement *settlement);

        FactionDecision Update();

        void Stop() {isActive = false;}

        bool IsActive() {return isActive;}
    };
}