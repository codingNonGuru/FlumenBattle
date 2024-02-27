#include "RecruitHandler.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/Race.h"
#include "FlumenBattle/World/Character/CharacterFactory.h"
#include "FlumenBattle/Utility/Utility.h"
#include "FlumenBattle/World/WorldTime.h"
#include "FlumenBattle/RaceFactory.h"

using namespace world::character;

#define MAXIMUM_RECRUIT_POOL 32

#define MAXIMUM_RECRUITS_PER_POOL 8

#define SPAWN_CHANCE_PER_TICK 1

#define DESPAWN_CHANCE_PER_TICK 1

#define BASE_ROLL_DC 101

static unsigned int uniqueId = 0;

static constexpr auto LAST_VISIT_EXPIRY_DURATION = world::WorldTime::HOUR_SIZE * world::WorldTime::HOURS_IN_DAY * 3;

RecruitHandler::RecruitHandler()
{
    recruitPools.Initialize(MAXIMUM_RECRUIT_POOL);

    for(int i = 0; i < recruitPools.GetCapacity(); ++i)
    {
        recruitPools.Add()->recruits.Initialize(MAXIMUM_RECRUITS_PER_POOL);
    }

    recruitPools.Reset();
}

container::Pool <RecruitData> &RecruitHandler::GetRecruitPool(const settlement::Settlement *settlement)
{
    auto pool = recruitPools.Find(settlement);
    if(pool == nullptr)
    {
        pool = recruitPools.Add();

        GeneratePool(pool, settlement);
    }

    return pool->recruits;
}

void RecruitHandler::GeneratePool(RecruitPool *pool, const settlement::Settlement *settlement)
{
    auto recruitPower = GetPotentialPower(settlement);

    pool->settlement = settlement;

    pool->recruits.Reset();

    pool->timeSinceLastVisit = 0;

    for(int i = 0; i < recruitPower; ++i)
    {
        pool->AddRecruit();
    }
}

void RecruitHandler::RemoveRecruit(const settlement::Settlement *settlement, unsigned int uniqueId)
{
    auto pool = recruitPools.Find(settlement);

    pool->recruits.Remove(uniqueId);
}

int RecruitHandler::GetPotentialPower(const settlement::Settlement *settlement)
{
    const auto population = settlement->GetPopulation();

    if(population == 0)
        return 2;
    else if(population == 1)
        return 3;
    else if(population <= 5)
        return 4;
    else if(population <= 12)
        return 5;
    else if(population <= 20)
        return 6;
    else if(population <= 35)
        return 7;
    else
        return 8;
}

int RecruitHandler::GetMaximumLevel(const settlement::Settlement *settlement)
{
    const auto population = settlement->GetPopulation();

    if(population <= 2)
        return 1;
    else if(population <= 5)
        return 2;
    else if(population <= 10)
        return 3;
    else if(population <= 20)
        return 4;
    else if(population <= 35)
        return 5;
    else
        return 6;
}

bool RecruitHandler::Update(const settlement::Settlement *currentSettlement)
{
    bool hasCurrentSettlementChanged = false;

    for(auto &pool : recruitPools)
    {
        auto hasChanged = pool.Update();
        if(currentSettlement == pool.settlement && hasChanged == true)
        {
            hasCurrentSettlementChanged = true;
        }
    }

    for(auto &pool : recruitPools)
    {
        if(pool.settlement == currentSettlement)
        {
            pool.timeSinceLastVisit = 0;
        }
        else
        {
            pool.timeSinceLastVisit++;

            if(pool.timeSinceLastVisit >= LAST_VISIT_EXPIRY_DURATION)
            {
                recruitPools.RemoveAt(&pool);
            }
        }
    }

    return hasCurrentSettlementChanged;
}

int RecruitPool::GetPower() const
{
    return recruits.GetSize();
}

bool RecruitPool::Update()
{
    bool hasRemovedAny = UpdateRemovals();

    bool hasAddedAny = UpdateAdditions();

    return hasRemovedAny || hasAddedAny;
}

void RecruitPool::AddRecruit()
{
    const auto maximumLevel = RecruitHandler::GetMaximumLevel(settlement);

    auto level = utility::GetRandom(1, maximumLevel);

    auto cost = level * 100 + utility::GetRandom(-2, 4) * 10;

    static const container::Array <CharacterClasses> classes = {CharacterClasses::FIGHTER, CharacterClasses::CLERIC, CharacterClasses::RANGER, CharacterClasses::WIZARD};

    auto iconTextureId = utility::GetRandom(0, character::CharacterFactory::GetIconCount() - 1);

    auto race = [&]
    {
        if(utility::RollD100Dice() > 30)
        {
            return settlement->GetRace();
        }
        else if(settlement->GetLinks().GetSize() > 0)
        {
            if(utility::RollD100Dice() > 40)
            {
                auto randomNeighbor = settlement->GetLinks().GetRandom()->Other;

                return randomNeighbor->GetRace();
            }
            else
            {   
                return RaceFactory::BuildRandomRace();
            }
        }
        else
        {
            if(utility::RollD100Dice() > 60)
            {
                return settlement->GetRace();
            }
            else
            {
                return RaceFactory::BuildRandomRace();
            }
        }
    } ();

    *recruits.Add() = {uniqueId, *classes.GetRandom(), race, level, cost, iconTextureId};

    uniqueId++;
}

bool RecruitPool::UpdateRemovals()
{
    if(recruits.GetSize() == 0)
        return false;

    if(utility::RollD100Dice() < BASE_ROLL_DC - DESPAWN_CHANCE_PER_TICK)
        return false;

    auto randomRecruit = recruits.GetRandom();

    recruits.RemoveAt(randomRecruit);

    return true;
}

bool RecruitPool::UpdateAdditions()
{
    auto power = GetPower();

    auto potentialPower = RecruitHandler::GetPotentialPower(settlement);

    if(power >= potentialPower)
        return false;

    auto powerDelta = potentialPower - power;

    const auto spawnDifficulty = (BASE_ROLL_DC - SPAWN_CHANCE_PER_TICK) - powerDelta;

    if(utility::RollD100Dice() < spawnDifficulty)
        return false;

    AddRecruit();

    return true;
}