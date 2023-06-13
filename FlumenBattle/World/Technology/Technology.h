#pragma once

#include "FlumenEngine/Core/Singleton.h"

#include "FlumenBattle/Types.hpp"

namespace world
{
    class Settlement;
}

namespace world::science
{
    class TechnologyFactory;

    struct TechnologyType
    {
        Technologies Type;

        int ResearchDuration;

        void (*ApplyEffect) (Settlement &);

        TechnologyType(Technologies type, int researchDuration, void (*applyEffect)(Settlement &)) : 
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
        }
        
        bool HasDiscovered(Technologies technology) const
        {
            return discoveries[(int)technology];
        }

        void Update(Settlement &);

        void ApplyModifiers(Settlement &);
    };

    class TechnologyApplier
    {
        friend class TechnologyFactory;

        static void ApplyHandWashing(Settlement &);

        static void ApplyTrainedSentinels(Settlement &);

        static void ApplyMasonry(Settlement &);
    };

    class TechnologyFactory : public Singleton <TechnologyFactory>
    {
        const TechnologyType &BuildHandWashing()
        {
            static const TechnologyType tech = {Technologies::HAND_WASHING, 200, &TechnologyApplier::ApplyHandWashing};
            return tech;
        }

        const TechnologyType &BuildTrainedSentinels()
        {
            static const TechnologyType tech = {Technologies::TRAINED_SENTINELS, 300, &TechnologyApplier::ApplyTrainedSentinels};
            return tech;
        }

        const TechnologyType &BuildMasonry()
        {
            static const TechnologyType tech = {Technologies::MASONRY, 400, &TechnologyApplier::ApplyMasonry};
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