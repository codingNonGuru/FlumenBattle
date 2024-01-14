#include "SimulationMap.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/SimulationDomain.h"
#include "FlumenBattle/World/Group/Group.h"

using namespace world;

#define FIRST_DISTANCE_THRESHOLD 3.0f

#define SECOND_DISTANCE_THRESHOLD 5.0f

SimulationDomain *SimulationMap::GetDomain(int x, int y)
{
    return domains.Get(x / TILES_PER_SIMULATION_DOMAIN, y / TILES_PER_SIMULATION_DOMAIN);
}

SimulationDomain *SimulationMap::GetDomain(WorldTile *tile)
{
    return domains.Get(tile->SquareCoordinates.x / TILES_PER_SIMULATION_DOMAIN, tile->SquareCoordinates.y / TILES_PER_SIMULATION_DOMAIN);
}

void SimulationMap::Initialize()
{
    Update();
}

void SimulationMap::Update()
{
    auto playerGroup = WorldScene::Get()->GetPlayerGroup();

    auto playerLocation = playerGroup->GetTile();

    auto playerCoordinates = playerLocation->SquareCoordinates / TILES_PER_SIMULATION_DOMAIN;

    for(int x = 0; x < domains.GetWidth(); ++x)
    {
        for(int y = 0; y < domains.GetHeight(); ++y)
        {
            auto width = x - playerCoordinates.x;
            auto height = y - playerCoordinates.y;

            auto distance = sqrt(width * width + height * height);

            if(distance < FIRST_DISTANCE_THRESHOLD)
            {
                *domains.Get(x, y) = {SimulationLevels::ADVANCED};
            }
            else if(distance < SECOND_DISTANCE_THRESHOLD)
            {
                *domains.Get(x, y) = {SimulationLevels::MEDIUM};
            }
            else
            {
                *domains.Get(x, y) = {SimulationLevels::BASIC};
            }
        }
    }
}