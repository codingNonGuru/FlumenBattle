#pragma once

#include "FlumenCore/Singleton.h"
#include "FlumenCore/Conventions.hpp"

#include "FlumenBattle/World/Science/Types.h"

namespace world::settlement
{
    class Settlement;
}

namespace world::polity
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

        Word Name;

        void (*ApplyEffect) (settlement::Settlement &);

        TechnologyType(Technologies type, int researchDuration, Word name, void (*applyEffect)(settlement::Settlement &)) : 
        Type(type), 
        ResearchDuration(researchDuration), 
        Name(name),
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

        void Update(const polity::Polity &);

        void ApplyModifiers(settlement::Settlement &);

        const TechnologyType *GetTarget() const {return researchTarget;}

        int GetProgress() const {return researchProgress;}
    };

    class TechnologyApplier
    {
        friend class TechnologyFactory;

        static void ApplyHandWashing(settlement::Settlement &);

        static void ApplyTrainedSentinels(settlement::Settlement &);

        static void ApplyMasonry(settlement::Settlement &);

        static void ApplyWoodWorking(settlement::Settlement &);
    };

    class TechnologyFactory : public core::Singleton <TechnologyFactory>
    {
        const TechnologyType &BuildHandWashing()
        {
            static const TechnologyType tech = {Technologies::HAND_WASHING, 10000, "Hand washing", &TechnologyApplier::ApplyHandWashing};
            return tech;
        }

        const TechnologyType &BuildTrainedSentinels()
        {
            static const TechnologyType tech = {Technologies::TRAINED_SENTINELS, 14000, "Trained sentinels", &TechnologyApplier::ApplyTrainedSentinels};
            return tech;
        }

        const TechnologyType &BuildMasonry()
        {
            static const TechnologyType tech = {Technologies::MASONRY, 12000, "Masonry", &TechnologyApplier::ApplyMasonry};
            return tech;
        }

        const TechnologyType &BuildWoodWorking()
        {
            static const TechnologyType tech = {Technologies::WOOD_WORKING, 12000, "Wood working", &TechnologyApplier::ApplyWoodWorking};
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
                case Technologies::WOOD_WORKING:
                    return BuildWoodWorking();
            }
        }
    };
}