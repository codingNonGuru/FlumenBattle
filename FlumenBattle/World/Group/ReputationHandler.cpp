#include "ReputationHandler.h"
#include "FlumenBattle/World/Group/Reputation.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Settlement/Types.h"

using namespace world::group;

static constexpr auto MAXIMUM_REPUTATIONS_PER_HANDLER = 16;

ReputationHandler::ReputationHandler() : reputations(MAXIMUM_REPUTATIONS_PER_HANDLER)
{

}

void ReputationHandler::AddFactor(settlement::Settlement *settlement, int strength)
{
    static const auto &time = WorldScene::Get()->GetTime();

    auto reputation = reputations.Find(settlement);
    if(reputation == nullptr)
    {
        reputation = reputations.Add();

        *reputation = {settlement};
    }
    else
    {
        reputation->Update();
    }

    *reputation->Factors.Add() = {time.TotalHourCount, strength};
}

int ReputationHandler::GetReputation(settlement::Settlement *settlement) const
{
    auto reputation = reputations.Find(settlement);
    if(reputation == nullptr)
    {
        return 0;
    }

    return reputation->GetSum();
}

world::settlement::SettlementAttitudes ReputationHandler::GetAttitude(settlement::Settlement *settlement) const
{
    const auto reputation = GetReputation(settlement);

    if(reputation <= -30)
    {
        return settlement::SettlementAttitudes::HOSTILE;
    }
    else if(reputation <= -15)
    {
        return settlement::SettlementAttitudes::UNFRIENDLY;
    }
    else if(reputation <= -5)
    {
        return settlement::SettlementAttitudes::COLD;
    }
    else if(reputation < 5)
    {
        return settlement::SettlementAttitudes::INDIFFERENT;
    }
    else if(reputation < 15)
    {
        return settlement::SettlementAttitudes::OPEN;
    }
    else if(reputation < 30)
    {
        return settlement::SettlementAttitudes::WELCOMING;
    }
    else
    {
        return settlement::SettlementAttitudes::FRIENDLY;
    }
}