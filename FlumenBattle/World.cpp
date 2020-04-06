#include "World.hpp"
#include "Group.h"
#include "Battle.hpp"

World::World()
{
    bandits = new Group();

    caravan = new Group();

    battle = new Battle(bandits, caravan);
}

void World::Update()
{
    battle->Update();
}