#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/Camera.hpp"
#include "FlumenEngine/Core/Transform.hpp"
#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"

#include "FlumenBattle/Types.hpp"
#include "PathLabel.h"
#include "FlumenBattle/World/WorldController.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Polity.h"

using namespace world;

void PathLabel::HandleConfigure()
{
    auto color = Color::RED * 0.5f;
    //auto height = -float(size_.y) / 2.0f + 20.0f;
    label = ElementFactory::BuildText(
        {Size(100, 100), drawOrder_ + 1, {Position2(0.0f, 0.0f), this}},
        {{"JSLAncient", "Large"}, color, "1"}
    );
    label->SetAlignment(Text::Alignments::CENTER);
    label->Enable();
}

void PathLabel::HandleUpdate()
{
    //Configure(Size(100, 100), drawOrder_, {Position2()});

    auto weight = tile->GetPathData();

    Phrase text; 
    text << weight;
    label->Setup(text);

    auto camera = RenderManager::GetCamera(Cameras::WORLD);

    auto screenPosition = camera->GetScreenPosition({tile->Position.x, tile->Position.y, 0.0f});

    auto offset = size_.y / 2.0f;

    transform_->GetPosition().x = screenPosition.x;
    transform_->GetPosition().y = screenPosition.y;// + offset + 25.0f / camera->GetZoomFactor();
}