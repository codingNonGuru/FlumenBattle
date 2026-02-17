#include "Cohort.h"

using namespace world::settlement;

#define EXPERIENCE_GAIN_PER_TICK 1

void Cohort::Initialize(const struct Race *race)
{
    Race = race;

    Health = MAXIMUM_HEALTH;

    Experience = 0;

    IsHired = false;

    Job = nullptr;
}

void Cohort::IncreaseExperience(int amount)
{
    Experience += amount;

    if(Experience > 15000)
    {
        Experience = 15000;
    }
}

void Cohort::Update()
{
    if(Job != nullptr)
    {
        IncreaseExperience(EXPERIENCE_GAIN_PER_TICK);
    }
}

int Cohort::GetLevel() const
{
    if(Experience < 1000)
    {
        return 1;
    }
    else if(Experience < 2500)
    {
        return 2;
    }
    else if(Experience < 5000)
    {
        return 3;
    }
    else if(Experience < 9000)
    {
        return 4;
    }
    else 
    {
        return 5;
    }
}

float Cohort::GetExperienceRatio() const
{
    if(Experience < 1000)
    {
        return (float)Experience / 1000.0f;
    }
    else if(Experience < 2500)
    {
        return ((float)Experience - 1000.0f) / 1500.0f;
    }
    else if(Experience < 5000)
    {
        return ((float)Experience - 2500.0f) / 2500.0f;
    }
    else if(Experience < 9000)
    {
        return ((float)Experience - 5000.0f) / 4000.0f;
    }
    else 
    {
        return ((float)Experience - 9000.0f) / 6000.0f;
    }
}