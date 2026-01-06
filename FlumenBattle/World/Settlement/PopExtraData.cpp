#include "PopExtraData.h"
#include "FlumenBattle/World/Settlement/PopHandler.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/Cohort.h"
#include "FlumenBattle/Race.h"
#include "FlumenBattle/RaceFactory.h"

using namespace world::settlement;

void PopExtraData::Initialize(PopHandler *popHandler)
{
    this->popHandler = popHandler;

    for(auto &raceGroup : popHandler->GetRaces())
    {
        for(int i = 0; i < raceGroup.Size; ++i)
        {
            auto cohort = cohorts.Add();

            auto race = RaceFactory::BuildRace(raceGroup.Race);

            cohort->Initialize(race);
        }
    }
}

void PopExtraData::AddPopulation(RaceTypes raceType)
{
    auto cohort = cohorts.Add();

    auto race = RaceFactory::BuildRace(raceType);

    cohort->Initialize(race);
}

RaceTypes PopExtraData::KillRandomPopulation()
{
    auto cohort = cohorts.GetRandom();

    auto race = cohort->Race;

    cohorts.RemoveAt(cohort);

    return race->Type;
}