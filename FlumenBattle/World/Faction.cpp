#include "Faction.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/Utility/Utility.h"

using namespace world::polity;

void Faction::AddMember(settlement::Settlement *member)
{
    *members.Add() = member;
}

void Faction::SetLeader(settlement::Settlement *settlement) 
{
    leader = settlement;
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

    auto revoltCheck = utility::RollD20Dice(15);
    if(revoltCheck.IsAnyFailure() == true)
    {
        independenceDrive = 0;
        return {FactionDecisions::NONE};
    }

    return {FactionDecisions::DECLARE_INDEPENDENCE, this};
}