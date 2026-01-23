#include "Cohort.h"

using namespace world::settlement;

void Cohort::Initialize(const struct Race *race)
{
    Race = race;

    Health = MAXIMUM_HEALTH;

    Experience = 0;
}