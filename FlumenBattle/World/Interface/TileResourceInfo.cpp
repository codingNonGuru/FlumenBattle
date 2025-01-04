#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/Camera.hpp"

#include "TileResourceInfo.h"
#include "FlumenBattle/World/Settlement/Resource.h"
#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/World/Tile/WorldTile.h"
#include "FlumenBattle/World/Interface/Counter.h"
#include "FlumenBattle/World/WorldController.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/SettlementTile.h"

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
    static const auto camera = RenderManager::GetCamera(Cameras::WORLD);
    auto zoomFactor = camera->GetZoomFactor();

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

void TileResourceInfo::ResourceItem::Setup(int resourceAmount, bool hasBonus)
{
    if(resourceAmount == 1)
    {
        counter->Disable();
    }
    else
    {
        counter->Setup(resourceAmount);
        counter->Enable();

        if(hasBonus)
            counter->SetTextColor(Color::GREEN * 0.7f + Color::BLACK * 0.3f);
        else
            counter->SetTextColor(Color::BLACK);
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

void TileResourceInfo::Setup(tile::WorldTile *tile)
{
    this->tile = tile;

    FollowWorldPosition(&tilePosition, Cameras::WORLD);

    for(auto &item : items)
    {
        auto amount = tile->GetResource(item->resource);
        auto modifiedAmount = amount;
        if(WorldController::Get()->ShouldResourceDisplayIncludeBonus() == true)
        {
            const auto playerSettlement = WorldScene::Get()->GetPlayerSettlement();
            if(playerSettlement != nullptr && tile->GetOwner() == playerSettlement)
            {
                modifiedAmount += playerSettlement->GetTiles().Find(tile)->GetImprovementBonus(item->resource);
            }
        }

        if(amount == 0)
        {
            item->Disable();
        }
        else
        {
            item->Setup(modifiedAmount, modifiedAmount != amount);
            item->Enable();
        }
    }
}
