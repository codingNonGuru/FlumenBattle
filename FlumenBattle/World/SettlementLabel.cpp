#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/Camera.hpp"
#include "FlumenEngine/Core/Transform.hpp"
#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/LayoutGroup.h"
#include "FlumenEngine/Interface/ProgressBar.h"

#include "SettlementLabel.h"
#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/Path.h"
#include "FlumenBattle/World/Settlement/SettlementProduction.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/Settlement/Affliction.h"
#include "FlumenBattle/World/Group/GroupDynamics.h"
#include "FlumenBattle/World/WorldController.h"
#include "FlumenBattle/Utility/Pathfinder.h"

using namespace world::settlement;

auto color = Color::RED * 0.5f;

auto borderColor = Color::RED * 0.25f;

void SettlementLabel::ResourceWidget::HandleConfigure()
{
    Label = ElementFactory::BuildText(
        {Size(100, 100), drawOrder_ + 1, {Position2(), ElementAnchors::MIDDLE_RIGHT, ElementPivots::MIDDLE_LEFT, this}},
        {{"JSLAncient", "Small"}, color, "20"}
    );
    Label->SetAlignment(Text::Alignments::LEFT);
    Label->AdjustSize();
    Label->Enable();
}

void SettlementLabel::ResourceWidget::HandleUpdate()
{
    Phrase text;
    if(IsTrackingProduction == true)
    {
        text << Parent->settlement->GetResource(Resource)->GetProduction(*Parent->settlement);
    }
    else
    {
        text << Parent->settlement->GetResource(Resource)->Storage;
    }
    Label->Setup(text);
}

void SettlementLabel::HandleConfigure() 
{
    this->settlement = nullptr;

    this->SetSpriteColor(borderColor);

    auto height = -float(size_.y) / 2.0f + 20.0f;
    nameLabel = ElementFactory::BuildText(
        {Size(100, 100), drawOrder_ + 1, {Position2(10.0f, 3.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}},
        {{"JSLAncient", "Medium"}, color, "Aloha"}
    );
    nameLabel->SetAlignment(Text::Alignments::LEFT);
    nameLabel->AdjustSize();
    nameLabel->Enable();

    backdrop = ElementFactory::BuildElement <Element>(
        {size_, drawOrder_ - 1, {Position2(0.0f, 0.0f), this}, {"Sprite"}, Opacity(0.5f)}
    );
    backdrop->Enable();

    populationBackdrop = ElementFactory::BuildElement <Element>(
        {Size(50, 50), drawOrder_ + 1, {Position2(-15.0f, 0.0f), ElementAnchors::MIDDLE_RIGHT, ElementPivots::MIDDLE_RIGHT, this}, {"Sprite"}, Opacity(0.5f)}
    );
    populationBackdrop->Enable();

    populationBorder = ElementFactory::BuildElement <Element>(
        {populationBackdrop->GetSize(), drawOrder_ + 2, {Position2(0.0f, 0.0f), populationBackdrop}, {"panel-border-015","SlicedSprite"}, Opacity(0.8f)}
    );
    populationBorder->SetSpriteColor(borderColor);
    populationBorder->Enable();

    populationLabel = ElementFactory::BuildText(
        {Size(50, 50), drawOrder_ + 2, {Position2(0.0f, 0.0f), populationBackdrop}},
        {{"JSLAncient", "Large"}, color, "5"}
    );
    populationLabel->SetAlignment(Text::Alignments::CENTER);
    populationLabel->Enable();

    hoverBackdrop = ElementFactory::BuildElement <Element>(
        {Size(size_.x - 10, 280), drawOrder_, {Position2(0.0f, 10.0f), ElementAnchors::LOWER_CENTER, ElementPivots::UPPER_CENTER, this}, {"Sprite"}, Opacity(0.4f)}
    );
    hoverBackdrop->Disable();

    hoverBorder = ElementFactory::BuildElement <Element>(
        {hoverBackdrop->GetSize(), drawOrder_ + 1, {Position2(0.0f, 0.0f), hoverBackdrop}, {"panel-border-007", "SlicedSprite"}, Opacity(0.8f)}
    );
    hoverBorder->SetSpriteColor(borderColor);
    hoverBorder->Enable();

    auto basePosition = Position2(8.0f, 23.0f);

    growthLabel = ElementFactory::BuildText(
        {Size(100, 100), drawOrder_ + 1, {basePosition, ElementAnchors::UPPER_LEFT, ElementPivots::MIDDLE_LEFT, hoverBackdrop}},
        {{"JSLAncient", "Small"}, color, "Growth "}
    );
    growthLabel->SetAlignment(Text::Alignments::LEFT);
    growthLabel->AdjustSize();
    growthLabel->Enable();
    basePosition.y += 20.0f;

    growthProgress = ElementFactory::BuildProgressBar <ProgressBar>(
        {Size(96, 16), drawOrder_ + 1, {Position2(), ElementAnchors::MIDDLE_RIGHT, ElementPivots::MIDDLE_LEFT, growthLabel}, {"Settings", "SlicedSprite"}},
        {"SettingsBar", {20.0f, 8.0f}}
    );
    growthProgress->Enable();

    industryLabel = ElementFactory::BuildText(
        {Size(100, 100), drawOrder_ + 1, {basePosition, ElementAnchors::UPPER_LEFT, ElementPivots::MIDDLE_LEFT, hoverBackdrop}},
        {{"JSLAncient", "Small"}, color, "Industry: 20"}
    );
    industryLabel->SetAlignment(Text::Alignments::LEFT);
    industryLabel->AdjustSize();
    industryLabel->Enable();
    basePosition.y += 20.0f;

    tileLabel = ElementFactory::BuildText(
        {Size(100, 100), drawOrder_ + 1, {basePosition, ElementAnchors::UPPER_LEFT, ElementPivots::MIDDLE_LEFT, hoverBackdrop}},
        {{"JSLAncient", "Small"}, color, "Tiles: 20"}
    );
    tileLabel->SetAlignment(Text::Alignments::LEFT);
    tileLabel->AdjustSize();
    tileLabel->Enable();
    basePosition.y += 10.0f;

    storageLayout = ElementFactory::BuildElement <LayoutGroup>(
        {Size(0, 0), drawOrder_ + 1, {basePosition, ElementAnchors::UPPER_LEFT, ElementPivots::UPPER_LEFT, hoverBackdrop}}
    );
    storageLayout->Enable();
    storageLayout->SetDistancing(3, 30.0f, -5.0f);

    struct Data
    {
        ResourceWidget **Widget;

        ResourceTypes Type;

        Word Texture;

        bool IsTrackingProduction {false};
    };

    Data resources[] = {
        {&foodWidget, ResourceTypes::FOOD, "Radish"}, {&timberWidget, ResourceTypes::TIMBER, "Timber"}, {&metalWidget, ResourceTypes::METAL, "Metal"},
        {&foodProduction, ResourceTypes::FOOD, "Radish", true}, {&timberProduction, ResourceTypes::TIMBER, "Timber", true}, {&metalProduction, ResourceTypes::METAL, "Metal", true}
        };

    for(auto resource : resources)
    {
        *resource.Widget = ElementFactory::BuildElement <ResourceWidget>(
            {Size(32, 32), drawOrder_ + 1, {basePosition, ElementAnchors::UPPER_LEFT, ElementPivots::MIDDLE_LEFT, storageLayout}, {resource.Texture, "Sprite"}, Opacity(1.0f)}
        );
        (*resource.Widget)->Resource = resource.Type;
        (*resource.Widget)->Parent = this;
        (*resource.Widget)->IsTrackingProduction = resource.IsTrackingProduction;
        (*resource.Widget)->Enable();

        storageLayout->AddChild(*resource.Widget);
    }
    basePosition.y += 70.0f;

    productionLabel = ElementFactory::BuildText(
        {Size(100, 100), drawOrder_ + 1, {basePosition, ElementAnchors::UPPER_LEFT, ElementPivots::MIDDLE_LEFT, hoverBackdrop}},
        {{"JSLAncient", "Small"}, color, "Industry: 20"}
    );
    productionLabel->SetAlignment(Text::Alignments::LEFT);
    productionLabel->AdjustSize();
    productionLabel->Enable();
    basePosition.y += 20.0f;

    productionProgress = ElementFactory::BuildProgressBar <ProgressBar>(
        {Size(192, 16), drawOrder_ + 1, {Position2(0.0f, basePosition.y), ElementAnchors::UPPER_CENTER, ElementPivots::MIDDLE_CENTER, hoverBackdrop}, {"Settings", "SlicedSprite"}},
        {"SettingsBar", {20.0f, 8.0f}}
    );
    productionProgress->Enable();
    basePosition.y += 15.0f;

    pathLayout = ElementFactory::BuildElement <LayoutGroup>(
        {Size(0, 0), drawOrder_ + 1, {basePosition, ElementAnchors::UPPER_LEFT, ElementPivots::UPPER_LEFT, hoverBackdrop}}
    );
    pathLayout->Enable();
    pathLayout->SetDistancing(1, -3.0f);
    basePosition.y += 20.0f;

    pathLabels.Initialize(8);
    for(int i = 0; i < 8; ++i)
    {
        auto label = ElementFactory::BuildText(
            {Size(100, 100), drawOrder_ + 1, {Position2(), pathLayout}},
            {{"JSLAncient", "VerySmall"}, color, "Industry: 20"}
        );
        label->SetAlignment(Text::Alignments::LEFT);
        label->AdjustSize();
        pathLayout->AddChild(label);
        label->Disable();

        *pathLabels.Add() = label;
    }

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

    text = "Growth ";
    //text << settlement->GetGrowth();
    growthLabel->Setup(text);

    growthProgress->SetProgress(settlement->GetGrowthRatio());

    text = "Industry: ";
    text << settlement->GetIndustrialProduction();
    industryLabel->Setup(text);

    text = "Tiles: ";
    text << settlement->GetWorkedTiles();
    tileLabel->Setup(text);

    text = "Building: ";
    text << settlement->GetCurrentProduction()->GetName();
    productionLabel->Setup(text);

    auto progress = settlement->GetCurrentProduction()->GetProgressRatio();
    if(settlement->GetCurrentProduction()->Is(SettlementProductionOptions::NONE) == true)
    {
        progress = 0.0f;
    }
    productionProgress->SetProgress(progress);

    int i = 0;
    for(auto &link : settlement->links)
    {
        auto label = pathLabels.Get(i);
        text = "";
        text << link.Path->Complexity << " to ";
        text << link.Path->GetOther(settlement)->GetName();
        (*label)->Setup(text);
        (*label)->Enable();
        i++;
    }

    //auto pathData = utility::Pathfinder <WorldTile>::Get()->FindPathToSettlement(settlement, settlement->GetRuler());
    //std::cout<<pathData.Length<<" "<<pathData.Complexity<<"\n";

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