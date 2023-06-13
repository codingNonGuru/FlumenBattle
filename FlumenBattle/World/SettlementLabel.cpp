#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/Camera.hpp"
#include "FlumenEngine/Core/Transform.hpp"
#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"

#include "SettlementLabel.h"
#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/Settlement/Affliction.h"
#include "FlumenBattle/World/Group/GroupDynamics.h"

using namespace world::settlement;

void SettlementLabel::HandleConfigure() 
{
    this->settlement = nullptr;
}

void SettlementLabel::SetSettlement(Settlement *settlement)
{
    this->settlement = settlement;

    auto color = Color::RED * 0.5f;
    auto height = -float(size_.y) / 2.0f + 20.0f;
    nameLabel = ElementFactory::BuildText(
        {Size(size_.x - 10, 150), drawOrder_ + 1, {Position2(0.0f, height), this}},
        {{"JSLAncient", "Small"}, color, settlement->GetName().Get()}
    );
    nameLabel->SetAlignment(Text::Alignments::LEFT);
    nameLabel->Enable();

    populationLabel = ElementFactory::BuildText(
        {Size(size_.x - 10, 150), drawOrder_ + 1, {Position2(0.0f, height + 20.0f), this}},
        {{"JSLAncient", "Small"}, color, "Pop: 1"}
    );
    populationLabel->SetAlignment(Text::Alignments::LEFT);
    populationLabel->Enable();

    growthLabel = ElementFactory::BuildText(
        {Size(size_.x - 10, 150), drawOrder_ + 1, {Position2(0.0f, height + 40.0f), this}},
        {{"JSLAncient", "Small"}, color, "Growth: 20"}
    );
    growthLabel->SetAlignment(Text::Alignments::LEFT);
    growthLabel->Enable();

    foodLabel = ElementFactory::BuildText(
        {Size(size_.x - 10, 150), drawOrder_ + 1, {Position2(0.0f, height + 60.0f), this}},
        {{"JSLAncient", "Small"}, color, "Food: 20"}
    );
    foodLabel->SetAlignment(Text::Alignments::LEFT);
    foodLabel->Enable();

    industryLabel = ElementFactory::BuildText(
        {Size(size_.x - 10, 150), drawOrder_ + 1, {Position2(0.0f, height + 80.0f), this}},
        {{"JSLAncient", "Small"}, color, "Food: 20"}
    );
    industryLabel->SetAlignment(Text::Alignments::LEFT);
    industryLabel->Enable();

    tileLabel = ElementFactory::BuildText(
        {Size(size_.x - 10, 150), drawOrder_ + 1, {Position2(0.0f, height + 100.0f), this}},
        {{"JSLAncient", "Small"}, color, "Food: 20"}
    );
    tileLabel->SetAlignment(Text::Alignments::LEFT);
    tileLabel->Enable();

    healthLabel = ElementFactory::BuildText(
        {Size(size_.x - 10, 150), drawOrder_ + 1, {Position2(0.0f, height + 120.0f), this}},
        {{"JSLAncient", "Small"}, color, "Food: 20"}
    );
    healthLabel->SetAlignment(Text::Alignments::LEFT);
    healthLabel->Enable();

    groupLabel = ElementFactory::BuildText(
        {Size(size_.x - 10, 150), drawOrder_ + 1, {Position2(0.0f, height + 140.0f), this}},
        {{"JSLAncient", "Small"}, color, "Food: 20"}
    );
    groupLabel->SetAlignment(Text::Alignments::LEFT);
    groupLabel->Enable();
}

void SettlementLabel::HandleUpdate() 
{
    if(this->settlement == nullptr)
        return;

    auto camera = RenderManager::GetCamera(Cameras::WORLD);

    auto location = settlement->GetLocation();
    auto screenPosition = camera->GetScreenPosition({location->Position.x, location->Position.y, 0.0f});

    auto offset = size_.y / 2.0f;

    transform_->GetPosition().x = screenPosition.x;
    transform_->GetPosition().y = screenPosition.y + offset + 25.0f / camera->GetZoomFactor();

    Phrase text = "Pop: ";
    text << settlement->GetPopulation();
    populationLabel->Setup(text);

    text = "Growth: ";
    text << settlement->GetGrowth();
    growthLabel->Setup(text);

    text = "Food: ";
    text << settlement->GetFoodProduction();
    foodLabel->Setup(text);

    text = "Industry: ";
    text << settlement->GetIndustrialProduction();
    industryLabel->Setup(text);

    text = "Tiles: ";
    text << settlement->GetWorkedTiles();
    tileLabel->Setup(text);

    auto malaria = settlement->afflictions.Find(AfflictionTypes::MALARIA);

    text = "Malaria? ";
    text << (malaria != nullptr ? "Yes" : "No");
    if(malaria != nullptr)
    {
        text << " / ";
        text << malaria->Stage;
    }
    healthLabel->Setup(text);

    text = "Groups   ";
    text << settlement->groupDynamics->patrolStrength << " / " << settlement->groupDynamics->banditStrength << " / " <<settlement->groupDynamics->merchantStrength;
    groupLabel->Setup(text);
}