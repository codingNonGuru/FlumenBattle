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
#include "FlumenBattle/World/WorldController.h"

using namespace world::settlement;

void SettlementLabel::HandleConfigure() 
{
    this->settlement = nullptr;

    auto color = Color::RED * 0.5f;
    auto height = -float(size_.y) / 2.0f + 20.0f;
    nameLabel = ElementFactory::BuildText(
        {Size(100, 100), drawOrder_ + 1, {Position2(5.0f, 0.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}},
        {{"JSLAncient", "Medium"}, color, "Aloha"}
    );
    nameLabel->SetAlignment(Text::Alignments::LEFT);
    nameLabel->AdjustSize();
    nameLabel->Enable();

    populationBackdrop = ElementFactory::BuildElement <Element>(
        {Size(50, 50), drawOrder_ + 1, {Position2(-5.0f, 0.0f), ElementAnchors::MIDDLE_RIGHT, ElementPivots::MIDDLE_RIGHT, this}, {"Sprite"}, Opacity(0.4f)}
    );
    populationBackdrop->Enable();

    populationLabel = ElementFactory::BuildText(
        {Size(50, 50), drawOrder_ + 2, {Position2(0.0f, 0.0f), populationBackdrop}},
        {{"JSLAncient", "Large"}, color, "5"}
    );
    populationLabel->SetAlignment(Text::Alignments::CENTER);
    populationLabel->Enable();

    hoverBackdrop = ElementFactory::BuildElement <Element>(
        {Size(size_.x - 10, 100), drawOrder_, {Position2(0.0f, 10.0f), ElementAnchors::LOWER_CENTER, ElementPivots::UPPER_CENTER, this}, {"Sprite"}, Opacity(0.4f)}
    );
    hoverBackdrop->Disable();

    growthLabel = ElementFactory::BuildText(
        {Size(100, 100), drawOrder_ + 1, {Position2(5.0f, 5.0f), ElementAnchors::UPPER_LEFT, ElementPivots::UPPER_LEFT, hoverBackdrop}},
        {{"JSLAncient", "Small"}, color, "Growth: 20"}
    );
    growthLabel->SetAlignment(Text::Alignments::LEFT);
    growthLabel->AdjustSize();
    growthLabel->Enable();

    foodLabel = ElementFactory::BuildText(
        {Size(100, 100), drawOrder_ + 1, {Position2(5.0f, 25.0f), ElementAnchors::UPPER_LEFT, ElementPivots::UPPER_LEFT, hoverBackdrop}},
        {{"JSLAncient", "Small"}, color, "Food: 20"}
    );
    foodLabel->SetAlignment(Text::Alignments::LEFT);
    foodLabel->AdjustSize();
    foodLabel->Enable();

    storageLabel = ElementFactory::BuildText(
        {Size(100, 100), drawOrder_ + 1, {Position2(5.0f, 45.0f), ElementAnchors::UPPER_LEFT, ElementPivots::UPPER_LEFT, hoverBackdrop}},
        {{"JSLAncient", "Small"}, color, "Food: 20"}
    );
    storageLabel->SetAlignment(Text::Alignments::LEFT);
    storageLabel->AdjustSize();
    storageLabel->Enable();

    industryLabel = ElementFactory::BuildText(
        {Size(100, 100), drawOrder_ + 1, {Position2(5.0f, 65.0f), ElementAnchors::UPPER_LEFT, ElementPivots::UPPER_LEFT, hoverBackdrop}},
        {{"JSLAncient", "Small"}, color, "Industry: 20"}
    );
    industryLabel->SetAlignment(Text::Alignments::LEFT);
    industryLabel->AdjustSize();
    industryLabel->Enable();

    /*foodLabel = ElementFactory::BuildText(
        {Size(size_.x - 10, 150), drawOrder_ + 1, {Position2(0.0f, height + 60.0f), this}},
        {{"JSLAncient", "Small"}, color, "Food: 20"}
    );
    foodLabel->SetAlignment(Text::Alignments::LEFT);
    foodLabel->Disable();*/

    /*industryLabel = ElementFactory::BuildText(
        {Size(size_.x - 10, 150), drawOrder_ + 1, {Position2(0.0f, height + 80.0f), this}},
        {{"JSLAncient", "Small"}, color, "Food: 20"}
    );
    industryLabel->SetAlignment(Text::Alignments::LEFT);
    industryLabel->Disable();*/

    tileLabel = ElementFactory::BuildText(
        {Size(size_.x - 10, 150), drawOrder_ + 1, {Position2(0.0f, height + 100.0f), this}},
        {{"JSLAncient", "Small"}, color, "Food: 20"}
    );
    tileLabel->SetAlignment(Text::Alignments::LEFT);
    tileLabel->Disable();

    healthLabel = ElementFactory::BuildText(
        {Size(size_.x - 10, 150), drawOrder_ + 1, {Position2(0.0f, height + 120.0f), this}},
        {{"JSLAncient", "Small"}, color, "Food: 20"}
    );
    healthLabel->SetAlignment(Text::Alignments::LEFT);
    healthLabel->Disable();

    groupLabel = ElementFactory::BuildText(
        {Size(size_.x - 10, 150), drawOrder_ + 1, {Position2(0.0f, height + 140.0f), this}},
        {{"JSLAncient", "Small"}, color, "Food: 20"}
    );
    groupLabel->SetAlignment(Text::Alignments::LEFT);
    groupLabel->Disable();
}

void SettlementLabel::SetSettlement(Settlement *settlement)
{
    this->settlement = settlement;
}

bool SettlementLabel::IsSettlementHovered()
{
    if(this->settlement == nullptr)
        return false;

    auto hoveredTile = world::WorldController::Get()->GetHoveredTile();
    return hoveredTile != nullptr && hoveredTile->GetSettlement() == this->settlement;
}

void SettlementLabel::HandleUpdate() 
{
    if(this->settlement == nullptr)
        return;

    if(IsSettlementHovered() == true)
    {
        hoverBackdrop->Enable();
    }
    else
    {
        hoverBackdrop->Disable();
    }

    auto camera = RenderManager::GetCamera(Cameras::WORLD);

    auto location = settlement->GetLocation();
    auto screenPosition = camera->GetScreenPosition({location->Position.x, location->Position.y, 0.0f});

    auto offset = size_.y / 2.0f;

    transform_->GetPosition().x = screenPosition.x;
    transform_->GetPosition().y = screenPosition.y + offset + 25.0f / camera->GetZoomFactor();

    Phrase text;
    text << settlement->GetName().Get();
    nameLabel->Setup(text);

    text = "";
    text << settlement->GetPopulation();
    populationLabel->Setup(text);

    text = "Growth: ";
    text << settlement->GetGrowth();
    growthLabel->Setup(text);

    text = "Food prod: ";
    text << settlement->GetFoodProduction();
    foodLabel->Setup(text);

    text = "Food stored: ";
    text << settlement->GetFoodStorage();
    storageLabel->Setup(text);

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