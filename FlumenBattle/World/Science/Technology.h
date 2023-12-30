#pragma once

#include "FlumenCore/Singleton.h"

#include "FlumenBattle/World/Science/Types.h"

namespace world::settlement
{
    class Settlement;
}

namespace world
{
    class Polity;
}

namespace world::science
{
    class TechnologyFactory;

    struct TechnologyType
    {
        Technologies Type;

        int ResearchDuration;

        void (*ApplyEffect) (settlement::Settlement &);

        TechnologyType(Technologies type, int researchDuration, void (*applyEffect)(settlement::Settlement &)) : 
        Type(type), 
        ResearchDuration(researchDuration), 
        ApplyEffect(applyEffect) {}

        TechnologyType(const TechnologyType &) = delete;

        TechnologyType & operator= (const TechnologyType &) = delete;
    };

    class TechnologyRoster
    {
        static constexpr int TECHNOLOGY_COUNT = (int)Technologies::COUNT;

        bool discoveries[TECHNOLOGY_COUNT];

        const TechnologyType *researchTarget;

        int researchProgress;

        void DiscoverTechnology(Technologies technology) 
        {
            discoveries[(int)technology] = true;
        }

    public:
        TechnologyRoster()
        {
            for(auto &technology : discoveries)
            {
                technology = false;
            }

            researchTarget = nullptr;
        }

        bool HasDiscovered(Technologies technology) const
        {
            return discoveries[(int)technology];
        }

        bool IsResearchingAnything() const {return researchTarget != nullptr;}

        void StartResearching(Technologies);

        void Update(const Polity &);

        void ApplyModifiers(settlement::Settlement &);
    };

    class TechnologyApplier
    {
        friend class TechnologyFactory;

        static void ApplyHandWashing(settlement::Settlement &);

        static void ApplyTrainedSentinels(settlement::Settlement &);

        static void ApplyMasonry(settlement::Settlement &);
    };

    class TechnologyFactory : public core::Singleton <TechnologyFactory>
    {
        const TechnologyType &BuildHandWashing()
        {
            static const TechnologyType tech = {Technologies::HAND_WASHING, 500, &TechnologyApplier::ApplyHandWashing};
            return tech;
        }

        const TechnologyType &BuildTrainedSentinels()
        {
            static const TechnologyType tech = {Technologies::TRAINED_SENTINELS, 700, &TechnologyApplier::ApplyTrainedSentinels};
            return tech;
        }

        const TechnologyType &BuildMasonry()
        {
            static const TechnologyType tech = {Technologies::MASONRY, 600, &TechnologyApplier::ApplyMasonry};
            return tech;
        }

    public:
        const TechnologyType &Create(Technologies technology)
        {
            switch(technology)
            {
                case Technologies::HAND_WASHING:
                    return BuildHandWashing();
                case Technologies::TRAINED_SENTINELS:
                    return BuildTrainedSentinels();
                case Technologies::MASONRY:
                    return BuildMasonry();
            }
        }
    };
}