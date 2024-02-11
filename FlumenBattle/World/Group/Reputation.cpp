#include "Reputation.h"
#include "FlumenBattle/World/WorldScene.h"

using namespace world::group;

bool ReputationFactor::HasExpired() const
{
    static const auto &time = WorldScene::Get()->GetTime();

    auto timeElapsed = time.TotalHourCount - TimeAcquired;

    auto timeFactor = timeElapsed / WorldTime::HOURS_IN_DAY;

    auto strength = abs(BaseStrength) - timeFactor;

    if(strength <= 0)
        return true;
    else
        return false;
}

int ReputationFactor::GetStrength() const
{
    static const auto &time = WorldScene::Get()->GetTime();

    auto timeElapsed = time.TotalHourCount - TimeAcquired;
    
    auto timeFactor = timeElapsed / WorldTime::HOURS_IN_DAY;

    if(BaseStrength < 0)
    {
        auto strength = BaseStrength + timeFactor;
        if(strength > 0)
            strength = 0;

        return strength;
    }
    else
    {
        auto strength = BaseStrength - timeFactor;
        if(strength < 0)
            strength = 0;

        return strength;
    }
}

int Reputation::GetSum() const
{
    auto sum = 0;
    for(auto &factor : Factors)
    {
        sum += factor.GetStrength();
    }

    return sum;
}

void Reputation::Update()
{
    static const auto &time = WorldScene::Get()->GetTime();

    for(auto &factor : Factors)
    {
        if(factor.HasExpired() == false)
            continue;

        Factors.RemoveAt(&factor);
    }
}