#include "Technology.h"
#include "FlumenBattle/World/Settlement/Settlement.h"

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

void TechnologyRoster::Update(Settlement &settlement)
{
    researchProgress++;
    if(researchProgress >= researchTarget->ResearchDuration)
    {
        DiscoverTechnology(researchTarget->Type);
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