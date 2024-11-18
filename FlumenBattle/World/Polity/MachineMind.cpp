#include "MachineMind.h"
#include "FlumenBattle/World/Polity/Polity.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/SettlementProduction.h"

using namespace world::polity;

void MachineMind::MakeDecision(Polity &polity) const
{
    for(auto &settlement : polity.GetSettlements())
    {
        settlement->WorkNewTile();
    }

    for(auto &settlement : polity.GetSettlements())
    {
        if(settlement->GetCurrentProduction()->Is(settlement::ProductionOptions::NONE) == false)
            continue;

        settlement->DecideProduction();
    }
}