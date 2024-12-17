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
#include "FlumenBattle/World/Settlement/Condition.h"
#include "FlumenBattle/World/Settlement/Building.h"
#include "FlumenBattle/World/Group/GroupDynamics.h"
#include "FlumenBattle/World/WorldController.h"
#include "FlumenBattle/Utility/Pathfinder.h"
#include "FlumenBattle/World/Polity/Polity.h"
#include "FlumenBattle/World/Polity/Faction.h"
#include "FlumenBattle/WorldInterface.h"
#include "FlumenBattle/World/Interface/ResourceCounter.h"

using namespace world::settlement;

auto color = Color::RED * 0.5f;

auto borderColor = Color::RED * 0.25f;

void HoverExtension::ResourceWidget::HandleConfigure()
{
    Label = ElementFactory::BuildText(
        {Size(100, 100), drawOrder_ + 1, {Position2(), ElementAnchors::MIDDLE_RIGHT, ElementPivots::MIDDLE_LEFT, this}},
        {{"Small"}, color, "20"}
    );
    Label->SetAlignment(Text::Alignments::LEFT);
    Label->Enable();
}

void HoverExtension::ResourceWidget::HandleUpdate()
{
    Phrase text;
    if(IsTrackingProduction == true)
    {
        text << Parent->settlement->GetResource(Resource)->GetPotentialProduction(*Parent->settlement);
    }
    else
    {
        text << Parent->settlement->GetResource(Resource)->Storage;
    }
    Label->Setup(text);
}

void HoverExtension::ConditionWidget::HandleConfigure()
{
    Progress = ElementFactory::BuildProgressBar <ProgressBar>(
        {Size(48, 16), drawOrder_ + 1, {ElementAnchors::MIDDLE_RIGHT, ElementPivots::MIDDLE_LEFT, this}, {"BaseBar", true}},
        {"BaseFillerRed", {6.0f, 8.0f}}
    );
    Progress->Enable();
}

void HoverExtension::ConditionWidget::HandleUpdate()
{
    if(Condition->Type == nullptr)
    {
        Disable();
        return;
    }

    Phrase text;
    text << Condition->Type->Name;
    Setup(text);

    Progress->SetProgress((float)Condition->HoursElapsed / (float)Condition->Duration);
}

void HoverExtension::HandleEnable() 
{
    UpdatePosition();
    
    for(auto label : pathLabels)
    {
        label->Disable();
    }
}

void SettlementLabel::HandleConfigure() 
{
    this->settlement = nullptr;

    this->SetSpriteColor(borderColor);

    auto height = -float(size_.y) / 2.0f + 20.0f;
    nameLabel = ElementFactory::BuildText(
        {Size(100, 100), drawOrder_ + 1, {Position2(10.0f, 3.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}},
        {{"Medium"}, color, "PathData"}
    );
    nameLabel->SetAlignment(Text::Alignments::LEFT);
    nameLabel->Enable();

    backdrop = ElementFactory::BuildElement <Element>(
        {size_, drawOrder_ - 1, {Position2(0.0f, 0.0f), this}, {false}, Opacity(0.6f)}
    );
    backdrop->Enable();

    populationBackdrop = ElementFactory::BuildElement <Element>(
        {Size(50, 50), drawOrder_ + 1, {Position2(-15.0f, 0.0f), ElementAnchors::MIDDLE_RIGHT, ElementPivots::MIDDLE_RIGHT, this}, {false}, Opacity(0.6f)}
    );
    populationBackdrop->Enable();

    populationBorder = ElementFactory::BuildElement <Element>(
        {populationBackdrop->GetSize(), drawOrder_ + 2, {populationBackdrop}, {"panel-border-015", true}, Opacity(0.8f)}
    );
    populationBorder->SetSpriteColor(borderColor);
    populationBorder->Enable();

    populationLabel = ElementFactory::BuildText(
        {drawOrder_ + 2, {populationBackdrop}},
        {{"Large"}, color, "5"}
    );
    populationLabel->SetAlignment(Text::Alignments::CENTER);
    populationLabel->Enable();

    distanceRelatedBackdrop = ElementFactory::BuildElement <Element>(
        {Size(size_.x - 30, size_.y), drawOrder_, {Position2(0.0f, 10.0f), ElementAnchors::LOWER_CENTER, ElementPivots::UPPER_CENTER, this}, {false}, Opacity(0.6f)}
    );

    extensionBorder = ElementFactory::BuildElement <Element>(
        {distanceRelatedBackdrop->GetSize(), drawOrder_ + 1, {distanceRelatedBackdrop}, {"panel-border-015", true}, Opacity(0.8f)}
    );
    extensionBorder->SetSpriteColor(borderColor);
    extensionBorder->Enable();

    garrisonCounter = ElementFactory::BuildElement <world::interface::ResourceCounter> (
        {drawOrder_, {Position2(30.0f, 0.0f), distanceRelatedBackdrop}}
    );
    garrisonCounter->Setup("Group", &garrisonCount, "Small");
    garrisonCounter->Enable();

    wallCounter = ElementFactory::BuildElement <world::interface::ResourceCounter> (
        {drawOrder_, {Position2(-30.0f, 0.0f), distanceRelatedBackdrop}}
    );
    wallCounter->Setup("Walls", &wallsLevel, "Small", Scale2(0.5f));
    wallCounter->Enable();
}

static Text *separator;

void HoverExtension::HandleConfigure() 
{
    this->settlement = nullptr;

    border = ElementFactory::BuildElement <Element>(
        {size_, drawOrder_ + 1, {this}, {"panel-border-007", true}, Opacity(0.8f)}
    );
    border->SetSpriteColor(borderColor);
    border->Enable();

    auto basePosition = Position2(8.0f, 23.0f);

    growthLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {basePosition, ElementAnchors::UPPER_LEFT, ElementPivots::MIDDLE_LEFT, this}},
        {{"Small"}, color, "Growth "}
    );
    growthLabel->SetAlignment(Text::Alignments::LEFT);
    growthLabel->Enable();
    basePosition.y += 20.0f;

    simulationLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {basePosition, ElementAnchors::UPPER_LEFT, ElementPivots::MIDDLE_LEFT, this}},
        {{"Small"}, color, "Growth "}
    );
    simulationLabel->SetAlignment(Text::Alignments::LEFT);
    simulationLabel->Enable();
    basePosition.y += 20.0f;

    growthProgress = ElementFactory::BuildProgressBar <ProgressBar>(
        {Size(96, 16), drawOrder_ + 1, {ElementAnchors::MIDDLE_RIGHT, ElementPivots::MIDDLE_LEFT, growthLabel}, {"BaseBar", true}},
        {"BaseFillerRed", {6.0f, 8.0f}}
    );
    growthProgress->Enable();

    industryLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {basePosition, ElementAnchors::UPPER_LEFT, ElementPivots::MIDDLE_LEFT, this}},
        {{"Small"}, color, "Industry: 20"}
    );
    industryLabel->SetAlignment(Text::Alignments::LEFT);
    industryLabel->Enable();
    basePosition.y += 20.0f;

    tileLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {basePosition, ElementAnchors::UPPER_LEFT, ElementPivots::MIDDLE_LEFT, this}},
        {{"Small"}, color, "Tiles: 20"}
    );
    tileLabel->SetAlignment(Text::Alignments::LEFT);
    tileLabel->Enable();
    basePosition.y += 20.0f;

    factionLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {basePosition, ElementAnchors::UPPER_LEFT, ElementPivots::MIDDLE_LEFT, this}},
        {{"Small"}, color, "Faction: 20"}
    );
    factionLabel->SetAlignment(Text::Alignments::LEFT);
    factionLabel->Enable();
    basePosition.y += 20.0f;

    buildingLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {basePosition, ElementAnchors::UPPER_LEFT, ElementPivots::MIDDLE_LEFT, this}},
        {{"Small"}, color, "Built:"}
    );
    buildingLabel->SetAlignment(Text::Alignments::LEFT);
    buildingLabel->Enable();
    basePosition.y += 20.0f;

    groupLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {basePosition, ElementAnchors::UPPER_LEFT, ElementPivots::MIDDLE_LEFT, this}},
        {{"Small"}, color, "Groups:"}
    );
    groupLabel->SetAlignment(Text::Alignments::LEFT);
    groupLabel->Enable();
    basePosition.y += 10.0f;

    storageLayout = ElementFactory::BuildElement <LayoutGroup>(
        {drawOrder_, {basePosition, ElementAnchors::UPPER_LEFT, ElementPivots::UPPER_LEFT, this}}
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
            {Size(32, 32), drawOrder_ + 1, {storageLayout}, {resource.Texture, false}}
        );
        (*resource.Widget)->Resource = resource.Type;
        (*resource.Widget)->Parent = this;
        (*resource.Widget)->IsTrackingProduction = resource.IsTrackingProduction;
        (*resource.Widget)->Enable();
    }
    basePosition.y += 70.0f;

    productionLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {basePosition, ElementAnchors::UPPER_LEFT, ElementPivots::MIDDLE_LEFT, this}},
        {{"Small"}, color, "Industry: 20"}
    );
    productionLabel->SetAlignment(Text::Alignments::LEFT);
    productionLabel->Enable();
    basePosition.y += 20.0f;

    productionProgress = ElementFactory::BuildProgressBar <ProgressBar>(
        {Size(160, 16), drawOrder_ + 1, {Position2(0.0f, basePosition.y), ElementAnchors::UPPER_CENTER, ElementPivots::MIDDLE_CENTER, this}, {"BaseBar", true}},
        {"BaseFillerRed", {6.0f, 8.0f}}
    );
    productionProgress->Enable();
    basePosition.y += 15.0f;

    pathLayout = ElementFactory::BuildElement <LayoutGroup>(
        {drawOrder_ + 1, {Position2(0.0f, 15.0f), ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, productionProgress}}
    );
    pathLayout->Enable();
    pathLayout->SetDistancing(1, -3.0f);
    basePosition.y += 20.0f;

    pathLabels.Initialize(8);
    for(int i = 0; i < 8; ++i)
    {
        auto label = ElementFactory::BuildText(
            {drawOrder_ + 1, {pathLayout}},
            {{"VerySmall"}, color, "Industry: 20"}
        );
        label->SetAlignment(Text::Alignments::LEFT);
        label->Disable();

        *pathLabels.Add() = label;
    }

    separator = ElementFactory::BuildText(
        {drawOrder_ + 1, {pathLayout}},
        {{"VerySmall"}, color, "----------"}
    );
    separator->SetAlignment(Text::Alignments::LEFT);
    separator->Enable();

    conditionLayout = ElementFactory::BuildElement <LayoutGroup>(
        {drawOrder_ + 1, {ElementAnchors::LOWER_CENTER, ElementPivots::UPPER_CENTER, nullptr}}
    );
    conditionLayout->SetDynamicParent(pathLayout);
    conditionLayout->Enable();
    conditionLayout->SetDistancing(1, 0.0f);
    //basePosition.y += 20.0f;

    conditionLabels.Initialize(8);
    for(int i = 0; i < 8; ++i)
    {
        auto label = ElementFactory::BuildText <ConditionWidget>(
            {drawOrder_ + 1, {conditionLayout}},
            {{"VerySmall"}, color, "Industry: 20"}
        );
        label->SetAlignment(Text::Alignments::LEFT);
        label->Disable();

        *conditionLabels.Add() = label;
    }
}

void SettlementLabel::SetSettlement(const Settlement *settlement)
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
        auto extension = WorldInterface::Get()->GetHoverExtension();
        extension->SetDynamicParent(this);
        extension->SetSettlement(settlement);
        extension->Enable();
    }
    else
    {
        auto extension = WorldInterface::Get()->GetHoverExtension();
        extension->Disable();
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

    if(camera->GetZoomFactor() < 0.45f && IsSettlementHovered() == false)
    {
        distanceRelatedBackdrop->Enable();

        garrisonCount = settlement->GetGroupDynamics().GetGarrisonStrength();

        wallsLevel = settlement->GetBuildingCount(BuildingTypes::WALLS);
    }
    else
    {
        distanceRelatedBackdrop->Disable();
    }
}

void HoverExtension::HandleUpdate() 
{
    if(this->settlement == nullptr)
        return;

    Phrase text("Growth ");
    //text << settlement->GetGrowth();
    growthLabel->Setup(text);

    growthProgress->SetProgress(settlement->GetGrowthRatio());

    text = "Sim: ";
    text << (int)settlement->GetSimulationLevel() << ", " << (int)settlement->HasAvailableColonySpots();
    simulationLabel->Setup(text);

    text = "Industry: ";
    text << settlement->GetIndustrialProduction();
    industryLabel->Setup(text);

    text = "Tiles: ";
    text << settlement->GetWorkedTiles();
    tileLabel->Setup(text);

    text = "Faction: ";
    if(settlement->GetFaction() != nullptr)
    {
        text << settlement->GetFaction()->GetLeader()->GetName() << " (" << settlement->GetFaction()->GetMembers().GetSize() << ")";
    }
    else
    {
        text << "-";
    }
    factionLabel->Setup(text);

    text = "Built:";
    for(auto &building : settlement->GetBuildings())
    {
        text << " ";

        if(building.GetAmount() == 0)
            continue;

        switch(building.GetType())
        {
        case BuildingTypes::SEWAGE:
            text << "S";
            break;
        case BuildingTypes::GRANARY:
            text << "G";
            break;
        case BuildingTypes::IRRIGATION:
            text << "I";
            break;
        case BuildingTypes::LIBRARY:
            text << "L";
            break;
        case BuildingTypes::LUMBER_MILL:
            text << "LM";
            break;
        case BuildingTypes::WEAVING_MILL:
            text << "WM";
            break;
        case BuildingTypes::TAILORY:
            text << "T";
            break;
        case BuildingTypes::CARPENTER:
            text << "CP";
            break;
        case BuildingTypes::BAKERY:
            text << "BK";
            break;
        case BuildingTypes::MARKET:
            text << "MK";
            break;
        case BuildingTypes::HOUSING:
            text << settlement->GetBuildingCount(BuildingTypes::HOUSING);
            text << "H";
            break;
        case BuildingTypes::WALLS:
            text << "W";
            break;
        case BuildingTypes::KEEP:
            text << "K";
            break;
        }
    }

    auto improvementCount = 0;
    for(auto &tile : settlement->GetTiles())
    {
        if(tile.IsBuilt)
            improvementCount++;
    }

    if(improvementCount > 0)
    {
        text << " " << improvementCount << "F";
    }
    buildingLabel->Setup(text);

    text = "Groups: ";
    auto &groupDynamics = settlement->GetGroupDynamics();
    text << groupDynamics.GetPatrolStrength() << "P ";
    text << groupDynamics.GetBanditStrength() << "B ";
    text << groupDynamics.GetAdventurerStrength() << "A ";
    text << groupDynamics.GetMerchantStrength() << "M";
    groupLabel->Setup(text);

    text = "Building: ";
    text << settlement->GetCurrentProduction()->GetName();
    productionLabel->Setup(text);

    auto progress = settlement->GetCurrentProduction()->GetProgressRatio();
    if(settlement->GetCurrentProduction()->Is(ProductionOptions::NONE) == true)
    {
        progress = 0.0f;
    }
    productionProgress->SetProgress(progress);

    int i = 0;
    for(auto &link : settlement->GetLinks())
    {
        auto label = pathLabels.Get(i);
        text = "";
        text << link.Path->Complexity << " to ";
        text << link.Path->GetOther(settlement)->GetName();
        (*label)->Setup(text);
        (*label)->Enable();
        i++;
    }

    for(auto &label : conditionLabels)
    {
        label->Condition = nullptr;
        label->Disable();
    }

    auto label = conditionLabels.GetStart();
    for(auto &condition : settlement->GetConditions())
    {
        (*label)->Condition = &condition;
        (*label)->Enable();

        label++;
    }

    /*for(auto &condition : settlement->GetConditions())
    {
        bool hasFound = false;
        for(auto &label : conditionLabels)
        {
            if(label->Condition != nullptr && label->Condition == condition)
            {
                hasFound = true;
                break;
            }
        }

        if(hasFound == true)
            continue;

        for(auto &label : conditionLabels)
        {
            if(label->Condition == nullptr || label->Condition == nullptr)
            {
                label->Condition = &condition;
                label->Enable();
                break;
            }
        }
    }*/

    //auto pathData = utility::Pathfinder <WorldTile>::Get()->FindPathToSettlement(settlement, settlement->GetRuler());
    //std::cout<<pathData.Length<<" "<<pathData.Complexity<<"\n";

    /*auto malaria = settlement->afflictions.Find(AfflictionTypes::MALARIA);

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
    groupLabel->Setup(text);*/
}