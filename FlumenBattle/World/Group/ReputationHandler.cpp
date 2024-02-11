#include "ReputationHandler.h"
#include "FlumenBattle/World/Group/Reputation.h"
#include "FlumenBattle/World/WorldScene.h"

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