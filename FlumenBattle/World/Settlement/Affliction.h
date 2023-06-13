#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenEngine/Core/Singleton.h"

#include "FlumenBattle/World/Types.h"

namespace world
{
    class Settlement;
    struct Affliction;

    struct AfflictionResult
    {
        AfflictionTypes AfflictionType;

        AfflictionResultTypes Type;
    };

    /*struct AfflictionData
    {
        const Affliction &Affliction;
    };*/

    struct AfflictionType
    {
        AfflictionTypes Type;

        Integer Throw;

        Integer StageCount;

        Integer Duration;

        //bool (*onCheck) (Group &, const GroupActionData &);

        AfflictionResult (*OnPerform) (Settlement &, Affliction &);

        //int (*onGetDuration) (const Group &);

        //void (*onPerform) (Group &);
    };

    class AfflictionPerformer
    {
        friend class Settlement;

        friend class AfflictionTypeBuilder;

        static AfflictionResult PerformMalaria(Settlement &, Affliction &);

        static AfflictionResult PerformHunger(Settlement &, Affliction &);
    };

    class AfflictionTypeBuilder : public Singleton<AfflictionTypeBuilder>
    {
        const AfflictionType * BuildMalaria();

        const AfflictionType * BuildHunger();

    public:
        const AfflictionType * BuildAffliction(AfflictionTypes afflictionType);
    };

    struct Affliction
    {
        const AfflictionType *Type;

        Integer Stage;

        Integer TimeLapsed;

        bool operator ==(AfflictionTypes Type) {return this->Type->Type == Type;}
    };

    class AfflictionFactory : public Singleton<AfflictionFactory>
    {
        friend class Settlement;

        Affliction Create(AfflictionTypes Type);
    };
}