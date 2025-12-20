#include "Cohort.h"

using namespace world::settlement;

void Cohort::Initialize(const struct Race *race)
{
    Race = race;

    Health = 100;

    Experience = 0;
}