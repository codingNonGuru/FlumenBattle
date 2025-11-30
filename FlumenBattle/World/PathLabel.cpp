#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/Camera.hpp"
#include "FlumenEngine/Core/Transform.hpp"
#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"

#include "FlumenBattle/Types.hpp"
#include "PathLabel.h"
#include "FlumenBattle/World/WorldController.h"
#include "FlumenBattle/World/Tile/WorldTile.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Polity/Polity.h"
#include "FlumenBattle/World/Tile/WorldEdge.h"
#include "FlumenBattle/World/Tile/WorldMap.h"
#include "FlumenBattle/World/WorldScene.h"

using namespace world;

void PathLabel::HandleConfigure()
{
    SetOpacity(0.7f);

    auto color = Color::RED * 0.25f;
    //auto height = -float(size_.y) / 2.0f + 20.0f;
    label = ElementFactory::BuildText(
        {Size(100, 100), drawOrder_ + 1, {Position2(), this}},
        {{"VerySmall"}, color, "1"}
    );
    label->SetAlignment(Text::Alignments::CENTER);
    label->Enable();

    tile = nullptr;

    edge = nullptr;
}

void PathLabel::HandleUpdate()
{
    //Configure(Size(100, 100), drawOrder_, {Position2()});

    static const auto &scene = *WorldScene::Get();

    auto map = scene.GetWorldMap();

    auto coordinates = map->GetEdgeCoordinates(tile, otherTile);

    Phrase text; 
    text << coordinates.x << "\n" << coordinates.y;
    label->Setup(text);

    auto camera = RenderManager::GetCamera(Cameras::WORLD);

    auto middle = tile->Position * 0.5f + otherTile->Position * 0.5f;
    auto screenPosition = camera->GetScreenPosition({middle.x, middle.y, 0.0f});

    auto offset = size_.y / 2.0f;

    transform_->GetPosition().x = screenPosition.x;
    transform_->GetPosition().y = screenPosition.y;// + offset + 25.0f / camera->GetZoomFactor();
}