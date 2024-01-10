#include "Faction.h"
#include "FlumenBattle/World/WorldScene.h"

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

    if(independenceDrive < 100)
        return {FactionDecisions::NONE};

    return {FactionDecisions::DECLARE_INDEPENDENCE, this};
}