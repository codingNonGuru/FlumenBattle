#include "FlumenCore/Utility/Utility.hpp"

#include "Technology.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/WorldScene.h"

using namespace world::science;

void TechnologyApplier::ApplyHandWashing(Settlement &settlement)
{

}

void TechnologyApplier::ApplyTrainedSentinels(Settlement &settlement)
{

}

void TechnologyApplier::ApplyMasonry(Settlement &settlement)
{

}

void TechnologyRoster::StartResearching(Technologies technology)
{
    researchTarget = &TechnologyFactory::Get()->Create(technology);
}

void TechnologyRoster::Update(Settlement &settlement)
{
    if(researchTarget == nullptr)
        return;

    researchProgress += settlement.GetScienceProduction();

    if(researchProgress >= researchTarget->ResearchDuration)
    {
        if(WorldScene::Get()->GetTime().MinuteCount == 0 && utility::GetRandom(1, 100) <= 1)
        {
            DiscoverTechnology(researchTarget->Type);

            researchTarget = nullptr;

            researchProgress = 0;
        }
    }
}

void TechnologyRoster::ApplyModifiers(Settlement &settlement)
{
    Technologies technology = Technologies(0);
    for(auto discovery : discoveries)
    {
        if(discovery)
        {
            TechnologyFactory::Get()->Create(technology).ApplyEffect(settlement);
        }

        technology = Technologies(int(technology) + 1);
    }
}