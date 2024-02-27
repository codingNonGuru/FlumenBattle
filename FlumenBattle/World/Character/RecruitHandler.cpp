#include "RecruitHandler.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/Race.h"
#include "FlumenBattle/World/Character/CharacterFactory.h"

using namespace world::character;

#define MAXIMUM_RECRUIT_POOL 32

#define MAXIMUM_RECRUITS_PER_POOL 8

static unsigned int uniqueId = 0;

RecruitHandler::RecruitHandler()
{
    recruitPools.Initialize(MAXIMUM_RECRUIT_POOL);

    for(int i = 0; i < recruitPools.GetCapacity(); ++i)
    {
        recruitPools.Add()->recruits.Initialize(MAXIMUM_RECRUITS_PER_POOL);
    }

    recruitPools.Reset();
}

container::Pool <RecruitData> &RecruitHandler::GetRecruitPool(settlement::Settlement *settlement)
{
    auto pool = recruitPools.Find(settlement);
    if(pool == nullptr)
    {
        pool = recruitPools.Add();

        GeneratePool(pool, settlement);
    }
    else
    {
        auto power = pool->GetPower();

        auto potentialPower = GetPotentialPower(settlement);
    }

    return pool->recruits;
}

void RecruitHandler::GeneratePool(RecruitPool *pool, settlement::Settlement *settlement)
{
    auto recruitPower = GetPotentialPower(settlement);

    pool->settlement = settlement;

    pool->recruits.Reset();

    for(int i = 0; i < recruitPower; ++i)
    {
        auto level = utility::GetRandom(1, 4);

        auto cost = level * 100 + utility::GetRandom(-2, 4) * 10;

        static const container::Array <CharacterClasses> classes = {CharacterClasses::FIGHTER, CharacterClasses::CLERIC, CharacterClasses::RANGER, CharacterClasses::WIZARD};

        auto iconTextureId = utility::GetRandom(0, character::CharacterFactory::GetIconCount() - 1);

        *pool->recruits.Add() = {uniqueId, *classes.GetRandom(), settlement->GetRace(), level, cost, iconTextureId};

        uniqueId++;
    }
}

void RecruitHandler::RemoveRecruit(settlement::Settlement *settlement, unsigned int uniqueId)
{
    auto pool = recruitPools.Find(settlement);

    pool->recruits.Remove(uniqueId);
}

int RecruitPool::GetPower() const
{
    return recruits.GetSize();
}

int RecruitHandler::GetPotentialPower(settlement::Settlement *settlement)
{
    auto population = settlement->GetPopulation();

    return 5;

    if(population == 0)
        return 0;
    else if(population == 1)
        return 1;
    else if(population < 5)
        return 2;
    else if(population < 10)
        return 3;
    else if(population < 17)
        return 4;
    else if(population < 30)
        return 5;
    else if(population < 50)
        return 6;
    else
        return 7;
}
