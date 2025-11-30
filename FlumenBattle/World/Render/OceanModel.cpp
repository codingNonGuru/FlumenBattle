#include "FlumenEngine/Render/ShaderManager.hpp"
#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Render/Camera.hpp"
#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/DataBuffer.hpp"
#include "FlumenEngine/Render/LineRenderer.h"

#include "OceanModel.h"
#include "FoamSegmentData.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Tile/WorldMap.h"
#include "FlumenBattle/World/Tile/WorldTile.h"
#include "FlumenBattle/World/Tile/WorldBiome.h"

using namespace world::render;

#define SEGMENT_THICKNESS 15.0f

#define FOAM_OPACITY 0.6f

static world::WorldScene *scene = nullptr;

static world::tile::WorldMap *map = nullptr;

static container::Array <FoamSegmentData> foamSegments;

static engine::render::LineRenderer *lineRenderer = nullptr;

void OceanModel::Initialize()
{
    auto getCoastDistance = [&] (tile::WorldTile *first, tile::WorldTile *second)
    {
        auto closestDistance = 999999;

        auto neighbours = first->GetNearbyTiles();
        auto otherNeighbours = second->GetNearbyTiles();
        for(auto &coast : neighbours)
        {
            if(coast->Type == WorldTiles::SEA)
                continue;

            for(auto &otherCoast : otherNeighbours)
            {
                if(otherCoast->Type == WorldTiles::SEA)
                    continue;

                auto distance = coast->GetDistanceTo(*otherCoast);
                if(distance < closestDistance)
                    closestDistance = distance;
            }
        }

        return closestDistance;
    };

    auto isStraits = [&] (tile::WorldTile *tile)
    {
        auto neighbours = tile->GetNearbyTiles();
    };

    scene = WorldScene::Get();

    map = scene->GetWorldMap();

    for(auto &tile : map->GetTiles())
    {
        if(tile.IsCoastline == false || tile.Type != WorldTiles::SEA || tile.IsBay == true)
            continue;

        auto neighbours = tile.GetNearbyTiles();
        for(auto &neighbour : neighbours)
        {
            if(neighbour->Type != WorldTiles::SEA)
                continue;

            if(neighbour->IsBay == true || neighbour->IsCoastline == false)
                continue;

            if(getCoastDistance(&tile, neighbour) >= 3)
                continue;

            *foamSegments.Add() = {&tile, neighbour};  
        }
    }

    auto &positions = lineRenderer->GetPositions();
    positions.Reset();

    auto &rotations = lineRenderer->GetRotations();
    rotations.Reset();

    auto &lengths = lineRenderer->GetLengths();
    lengths.Reset();

    auto &thicknesses = lineRenderer->GetThicknesses();
    thicknesses.Reset();

    auto &colors = lineRenderer->GetColors();
    colors.Reset();

    for(auto &segment : foamSegments)
    {
        *positions.Add() = segment.First->Position * 0.5f + segment.Second->Position * 0.5f;

        auto to = segment.First->Position - segment.Second->Position;
        *rotations.Add() = atan2(to.y, to.x) + HALF_PI;

        *lengths.Add() = tile::WorldMap::WORLD_TILE_SIZE * 2.0f;

        *thicknesses.Add() = SEGMENT_THICKNESS;

        *colors.Add() = Color::WHITE;
    }

    lineRenderer->TransferData(positions, rotations, lengths, thicknesses, colors);
}

void OceanModel::Render()
{
    static auto camera = RenderManager::GetCamera(Cameras::WORLD);

    lineRenderer->Render(camera, FOAM_OPACITY, 0.5f);
}

engine::render::LineRenderer *OceanModel::GetFoamRenderer()
{
    return lineRenderer;
}

void OceanModel::SetFoamRenderer(engine::render::LineRenderer *renderer)
{
    lineRenderer = renderer;
}

container::Array <FoamSegmentData> &OceanModel::GetFoamSegments()
{
    return foamSegments;
}