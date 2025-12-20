#include "PopExtraData.h"
#include "FlumenBattle/World/Settlement/PopHandler.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/Cohort.h"
#include "FlumenBattle/Race.h"

using namespace world::settlement;

void PopExtraData::Initialize(PopHandler *popHandler)
{
    this->popHandler = popHandler;

    for(int i = 0; i < popHandler->GetPopulation(); ++i)
    {
        auto race = popHandler->GetSettlement()->GetRace();

        auto cohort = cohorts.Add();

        cohort->Initialize(race);
    }
}