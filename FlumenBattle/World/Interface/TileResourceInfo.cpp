#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/Camera.hpp"

#include "TileResourceInfo.h"
#include "FlumenBattle/World/Settlement/Resource.h"
#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/Interface/Counter.h"

using namespace world::interface;

void TileResourceInfo::ResourceItem::HandleConfigure() 
{
    counter = ElementFactory::BuildElement <Counter>
    (
        {drawOrder_ + 1, {this}, {"WhiteDotBackdrop", false}}
    );
}

void TileResourceInfo::ResourceItem::HandleUpdate() 
{
    auto zoomFactor = RenderManager::GetCamera(Cameras::WORLD)->GetZoomFactor();

    SetBasePosition(position / zoomFactor);

    SetTextureScale(Scale2(0.7f / zoomFactor));

    counter->SetTextScale(1.0f);

    counter->Setup(Scale2(0.35f / zoomFactor), "Large");

    counter->SetBasePosition(Position2(6.0f) / zoomFactor);
}

void TileResourceInfo::ResourceItem::Setup(settlement::ResourceTypes resource)
{
    this->resource = resource;

    const auto &texture = settlement::ResourceFactory::Get()->CreateType(resource)->TextureName;
    SetTexture(texture);
}

void TileResourceInfo::ResourceItem::Setup(int resourceAmount)
{
    if(resourceAmount == 1)
    {
        counter->Disable();
    }
    else
    {
        counter->Setup(resourceAmount);
        counter->Enable();
    }
}

void TileResourceInfo::HandleConfigure()
{
    float angle = 0.0f;

    items.Initialize(BASIC_RESOURCES_COUNT);
    for(int i = 0; i < BASIC_RESOURCES_COUNT; ++i)
    {
        auto x = cos(angle) * 17.0f;
        auto y = sin(angle) * 17.0f;

        auto item = ElementFactory::BuildElement <ResourceItem>
        (
            { 
                drawOrder_, 
                {Position2{x, y}, this}, 
                {"Radish", false},
                Opacity(1.0f)
            }
        );
        item->AdjustSizeToTexture();
        item->position = Position2{x, y};
        item->Setup(settlement::ResourceTypes(i));
        item->Enable();

        *items.Add() = item;

        angle += TWO_PI / (float)BASIC_RESOURCES_COUNT;
    }
}

void TileResourceInfo::HandleUpdate()
{
    if(tile == nullptr)
        return;

    tilePosition = tile->Position;
}

void TileResourceInfo::Setup(WorldTile *tile)
{
    this->tile = tile;

    FollowWorldPosition(&tilePosition, Cameras::WORLD);

    for(auto &item : items)
    {
        auto amount = tile->GetResource(item->resource);

        if(amount == 0)
        {
            item->Disable();
        }
        else
        {
            item->Setup(amount);
            item->Enable();
        }
    }
}
