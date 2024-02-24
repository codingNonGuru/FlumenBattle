#include "Faction.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/Utility/Utility.h"
#include "FlumenBattle/World/Polity/Polity.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/Condition.h"

using namespace world::polity;

void Faction::AddMember(settlement::Settlement *member)
{
    *members.Add() = member;
}

void Faction::SetLeader(settlement::Settlement *settlement) 
{
    leader = settlement;
}

void Faction::RemoveMember(settlement::Settlement *settlement)
{
    members.Remove(settlement);
}

FactionDecision Faction::Update()
{
    if(members.GetSize() < 3)
        return {FactionDecisions::NONE};

    independenceDrive++;

    if(independenceDrive < 200)
    {
        return {FactionDecisions::NONE};
    }

    auto bonus = members.GetSize() >= 5 ? 2 : 0;

    auto revoltCheck = utility::RollD20Dice(16, bonus);
    if(revoltCheck.IsAnyFailure() == true)
    {
        independenceDrive = 0;

        auto exclusionCount = revoltCheck.IsCriticalFailure() == true ? 2 : 1;
        auto exclusionIndex = 0;
        while(true)
        {
            auto repressedMember = *members.GetRandom();
            if(repressedMember == leader)
                continue;

            auto duration = 24 * (2 + utility::GetRandom(1, 4));

            repressedMember->AddCondition({settlement::Conditions::REPRESSED, 1, duration});

            members.Remove(repressedMember);

            repressedMember->SetFaction(nullptr);

            exclusionIndex++;
            if(exclusionIndex == exclusionCount)
            {
                break;
            }
        }

        return {FactionDecisions::NONE};
    }

    return {FactionDecisions::DECLARE_INDEPENDENCE, this};
}